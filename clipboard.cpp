#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>

namespace {

void gdk_log_handler(const gchar* log_domain, GLogLevelFlags log_level, const gchar* message, gpointer user_data) {
    // Suppress all GTK/GDK warnings and messages that are not critical errors
    if (!(log_level & (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_FLAG_FATAL))) {
        return;
    }
    // Only show actual errors
    g_log_default_handler(log_domain, log_level, message, user_data);
}

void suppress_gdk_warnings() {
    auto levels = static_cast<GLogLevelFlags>(G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG);
    g_log_set_handler("Gdk", levels, gdk_log_handler, nullptr);
    g_log_set_handler("Gtk", levels, gdk_log_handler, nullptr);
}

gboolean quit_loop_cb(gpointer data) {
    GMainLoop* loop = static_cast<GMainLoop*>(data);
    g_main_loop_quit(loop);
    return G_SOURCE_REMOVE;
}

void flush_gtk_events() {
    while (gtk_events_pending()) {
        gtk_main_iteration();
    }
}

} // namespace

int main(int argc, char* argv[]) {
    // Read all input from stdin
    std::string input;
    std::string line;

    while (std::getline(std::cin, line)) {
        input += line;
        input.push_back('\n');
    }

    if (!input.empty() && input.back() == '\n') {
        input.pop_back();
    }

    if (input.empty()) {
        std::cerr << "Error: No input provided" << std::endl;
        return 1;
    }

    // Suppress GTK warnings by redirecting stderr temporarily
    int stderr_fd = dup(STDERR_FILENO);
    int null_fd = open("/dev/null", O_WRONLY);
    dup2(null_fd, STDERR_FILENO);
    close(null_fd);

    suppress_gdk_warnings();

    if (!gtk_init_check(&argc, &argv)) {
        // Restore stderr for error messages
        dup2(stderr_fd, STDERR_FILENO);
        close(stderr_fd);
        std::cerr << "Error: Unable to initialize GTK. Ensure a graphical session is available." << std::endl;
        return 1;
    }

    GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    if (!clipboard) {
        dup2(stderr_fd, STDERR_FILENO);
        close(stderr_fd);
        std::cerr << "Error: Unable to access system clipboard." << std::endl;
        return 1;
    }

    gtk_clipboard_set_text(clipboard, input.c_str(), static_cast<gint>(input.size()));
    gtk_clipboard_store(clipboard);

    GtkClipboard* primary = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
    if (primary) {
        gtk_clipboard_set_text(primary, input.c_str(), static_cast<gint>(input.size()));
        gtk_clipboard_store(primary);
    }

    flush_gtk_events();

    GMainLoop* loop = g_main_loop_new(nullptr, FALSE);
    if (loop) {
        g_timeout_add(150, quit_loop_cb, loop);
        g_main_loop_run(loop);
        g_main_loop_unref(loop);
    } else {
        g_usleep(150 * 1000);
    }

    flush_gtk_events();

    gtk_clipboard_wait_is_text_available(clipboard);

    // Restore stderr
    dup2(stderr_fd, STDERR_FILENO);
    close(stderr_fd);

    return 0;
}