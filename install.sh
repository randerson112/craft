#!/bin/bash
set -e

echo "Installing Craft..."

# Check dependencies
command -v git  >/dev/null 2>&1 || { echo "Error: git is required"; exit 1; }
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is required"; exit 1; }

# Clone and build
git clone --depth 1 https://github.com/randerson112/craft.git /tmp/craft-install
cd /tmp/craft-install
cmake -S . -B build
cmake --build build

# Install
mkdir -p ~/.craft/bin
cp build/craft ~/.craft/bin/craft

# Add to PATH
SHELL_RC=""
if [ -f ~/.zshrc ]; then
    SHELL_RC=~/.zshrc
elif [ -f ~/.bashrc ]; then
    SHELL_RC=~/.bashrc
fi

if [ -n "$SHELL_RC" ]; then
    if ! grep -q ".craft/bin" "$SHELL_RC"; then
        echo 'export PATH="$HOME/.craft/bin:$PATH"' >> "$SHELL_RC"
        echo "Added ~/.craft/bin to PATH in $SHELL_RC"
    fi
fi

# Cleanup
rm -rf /tmp/craft-install

echo "Craft installed successfully!"
echo "Restart your terminal or run 'source $SHELL_RC' to use craft"