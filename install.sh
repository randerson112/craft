#!/usr/bin/env sh
set -e

echo "Installing Craft..."

# Check dependencies
command -v git >/dev/null 2>&1 || { echo "Error: git is required"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is required"; exit 1; }

# Temp dir
TMP_DIR="/tmp/craft-install"
rm -rf "$TMP_DIR"

# Clone and build
git clone --depth 1 https://github.com/randerson112/craft.git "$TMP_DIR"
cd "$TMP_DIR"

cmake -S . -B build
cmake --build build

# Find executable (robust)
CRAFT_BIN_PATH=$(find build -type f -name craft -perm -111 2>/dev/null | head -n 1)

if [ -z "$CRAFT_BIN_PATH" ]; then
    echo "Error: build failed, craft binary not found"
    exit 1
fi

# Install
INSTALL_DIR="$HOME/.craft/bin"
mkdir -p "$INSTALL_DIR"
cp "$CRAFT_BIN_PATH" "$INSTALL_DIR/craft"
chmod +x "$INSTALL_DIR/craft"

# Detect shell config file
if [ -n "$ZSH_VERSION" ]; then
    SHELL_RC="$HOME/.zshrc"
elif [ -n "$BASH_VERSION" ]; then
    if [ -f "$HOME/.bashrc" ]; then
        SHELL_RC="$HOME/.bashrc"
    else
        SHELL_RC="$HOME/.bash_profile"
    fi
else
    SHELL_RC="$HOME/.profile"
fi

# Add to PATH if not already present
if ! echo "$PATH" | grep -q "$HOME/.craft/bin"; then
    echo "export PATH=\"\$HOME/.craft/bin:\$PATH\"" >> "$SHELL_RC"
    echo "Added ~/.craft/bin to PATH in $SHELL_RC"
fi

# Cleanup
rm -rf "$TMP_DIR"

echo ""
echo "Craft installed successfully!"
echo "Run: craft"
echo "If not found, restart your terminal or run:"
echo "  source $SHELL_RC"