# Craft

A lightweight build and workflow tool for C/C++

C and C++ development has always required wrestling with CMake, configuring build systems, and manually managing dependencies. Craft eliminates that friction. You describe your project in a simple `craft.toml` file and Craft handles the rest by generating CMake configuration, managing dependencies, and providing a clean command-line interface that feels modern and smooth.

---

## How Craft Works
1. Define your project in `craft.toml`
2. Craft generates `CMakeLists.txt`
3. Dependencies are fetched automatically
4. CMake builds your project behind the scenes

---

## Quick Example
```bash
# Create a new project
craft project my_app
cd my_app

# Add a dependency
craft add --git https://github.com/raysan5/raylib.git --links raylib

# Build it
craft build

# Run it
craft run
```

---

## What Craft Solves

### One file defines your project
Describe your project in a simple, readable config:
```toml
[project]
name = "my_app"
version = "0.1.0"
language = "cpp"
cpp_standard = 17

[build]
type = "executable"
include_dirs = ["include"]
source_dirs = ["src"]
```
Craft turns this into a working `CMakeLists.txt` automatically.

### Dependency management that just works
Add dependecies to your project with a single command.
```bash
# Local Craft project
craft add --path ../physics_engine

# Git dependency
craft add --git https://github.com/raysan5/raylib --links raylib

# Remove it just as easily
craft remove raylib

# Update all dependencies
craft update
```
- Git repos are cloned automatically
- CMake wiring is handled for you

### Templates for fast project setup
Save any project as a reusable template and spin up new projects from it instantly.
```bash
craft template save my_game_template
craft project new_game --template my_game_template
```
Craft also comes with a few built-in templates for executables and libraries.

---

## Installation

### macOS & Linux
```bash
curl -fsSL https://raw.githubusercontent.com/randerson112/craft/main/install.sh | sh
```

### Windows (PowerShell)
```powershell
irm https://raw.githubusercontent.com/randerson112/craft/main/install.ps1 | iex
```

**Requirements:** git, cmake

### Verifying Installation
```bash
craft --version
craft help
```

---

## Documentation
- [Getting started guide](docs/getting-started.md) - installing and learning the basics of Craft
- [Commands](docs/commands/README.md) - full documentation for each command
- [craft.toml reference](docs/craft-toml.md) - learn how Craft's main configuration file works

## Contributing
Craft is open source and community driven. Contributions are welcome. Feel free to open issues, pull requests, or assign yourself to any open issues.
