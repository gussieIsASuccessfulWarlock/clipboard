# Clipboard Command

A robust C++ utility that reads from standard input and copies the content to the system clipboard. Works on multiple Linux distributions and desktop environments (X11 and Wayland).

## Releases

Pre-built executables are available in the [GitHub Releases](https://github.com/gussieIsASuccessfulWarlock/clipboard/releases) for various Linux distributions and architectures. These are automatically built and released for:

- Ubuntu (x86_64, ARM64)
- Debian (x86_64)
- Fedora (x86_64)
- CentOS (x86_64)
- Alpine (x86_64)

Download the appropriate binary for your system, make it executable, and place it in your PATH:

```bash
chmod +x clipboard-ubuntu-x86_64  # Replace with the actual file name
sudo mv clipboard-ubuntu-x86_64 /usr/local/bin/clipboard
```

This provides an easy installation method without needing to build from source.

## Usage

Pipe any command output to the clipboard:

```bash
# Copy contents of text files to clipboard
cat *.txt | ./clipboard

# Copy current directory listing to clipboard
ls -la | ./clipboard

# Copy the output of any command to clipboard
echo "Hello, World!" | ./clipboard

# Copy file contents to clipboard
cat myfile.txt | ./clipboard
```

After installation, you can use it from anywhere:

```bash
cat *.txt | clipboard
```

## Dependencies

This program links directly against GTK 3 to talk to the system clipboard without spawning external binaries. Install the GTK development package for your distribution:

### Ubuntu/Debian
```bash
sudo apt install libgtk-3-dev
```

### Red Hat/Fedora/CentOS
```bash
sudo dnf install gtk3-devel
```

### Arch Linux
```bash
sudo pacman -S gtk3
```

### Other distributions
Install the development package that provides `pkg-config` support for `gtk+-3.0`.

## Building

```bash
make
```

## Installation

To install the program to `/usr/local/bin/`:

```bash
make install
```

## How it works

GTK abstracts both X11 and Wayland clipboards, so the program can set the selection text directly through `GtkClipboard`. After copying the text it briefly spins a main loop to ensure the clipboard manager stores the data, allowing the process to exit immediately. GTK warnings are suppressed to provide a clean user experience.

## Supported Systems

- **Ubuntu** (all versions)
- **Red Hat Enterprise Linux** (RHEL)
- **Fedora**
- **CentOS**
- **Arch Linux**
- **Manjaro**
- **Other Linux distributions**

Works with both X11 and Wayland display servers.