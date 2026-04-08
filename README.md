# Craft

A lightweight build tool for C and C++ projects — think Cargo, but for C/C++.

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
craft add --path ../PhysicsEngine          # local Craft project
craft add --git https://github.com/raysan5/raylib --tag 5.5   # git dependency
craft remove raylib                        # remove it just as easily
craft update                               # update all dependencies
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

## Quick Start
```bash
craft project my_app
cd my_app
craft build
craft run
```

---

## Commands and Documentation

### `craft project <path>`
Creates a new project directory at the given path with a full project structure including `craft.toml`, `src/`, `include/`, and a starter main file.
```bash
craft project my_app
craft project --no-git
craft project my_app --lang c
craft project my_lib --template static-library
```

### `craft init [path]`
Initializes a Craft project in the current or specified directory. Behavior depends on what is already there:

**Empty directory** — generates `craft.toml`, `CMakeLists.txt`, `src/`, `include/`, and a starter main file using the specified or default template.

**Existing project** — scans the directory to detect language, source dirs, include dirs, and libraries. Generates `craft.toml` and `CMakeLists.txt` from what was found. Existing source files are never modified. If a `CMakeLists.txt` already exists it will be backed up to `CMakeLists.backup.cmake` before being replaced.
```bash
craft init
craft init my_app
craft init my_app --no-git
craft init --lang c
craft init --template static-library
```

If you are adopting an existing project, Craft will back up your `CMakeLists.txt` to `CMakeLists.backup.cmake`.

To migrate your dependencies to Craft:
```
find_package(SFML ...)     -> craft add --git https://github.com/SFML/SFML --links SFML::Graphics,...
add_subdirectory(../MyLib) -> craft add --path ../MyLib
FetchContent_Declare(...)  -> craft add --git <url>
```

For anything else use CMakeLists.extra.cmake as an escape hatch.

### `craft build`
Reads `craft.toml`, regenerates `CMakeLists.txt` if needed, fetches any missing git dependencies, and builds the project using CMake. Can be run from any subdirectory — Craft walks up the directory tree to find the project root.
```bash
craft build
```

### `craft run [executable]`
Runs the project executable. Defaults to the project name from `craft.toml`. Optionally accepts an executable name to run any binary.
```bash
craft run
craft run my_other_binary
```

### `craft gen <file>`
Generates a file with boilerplate code. Places headers in the first `include_dir` and sources in the first `source_dir` from `craft.toml`. Falls back to the current directory if not in a project.
```bash
craft gen utils.hpp
craft gen utils.cpp
craft gen math.h
craft gen math.c
```

### `craft clean`
Removes the `build/` directory and all compiled artifacts. Reports how many files and bytes were removed.
```bash
craft clean
```

### `craft add`
Adds a dependency to the project. Git dependencies are cloned to `.craft/deps/` automatically. Regenerates `CMakeLists.txt` immediately so the project is always in a consistent state.
```bash
# Local Craft dependency
craft add --path ../my_lib 

# Git dependency in registry
craft add raylib

# Git dependencies not in registry
craft add --git https://github.com/nlohmann/json --links nlohmann_json::nlohmann_json
craft add --git https://github.com/SFML/SFML --tag 3.0.0 --links SFML::Graphics,SFML::Window,SFML::System
```

### `craft remove <name>`
Removes a dependency from `craft.toml` and regenerates `CMakeLists.txt`. For git dependencies, the cloned directory in `.craft/deps/` is also deleted.
```bash
craft remove raylib
craft remove my_lib
```

### `craft update [name]`
Updates git dependencies to their latest version by re-cloning. Dependencies pinned to a tag are skipped. Optionally update a single dependency by name.
```bash
craft update
craft update raylib
```

### `craft template <subcommand>`
Manages reusable project templates. Craft ships with built-in templates for common project types. Custom templates are saved to `~/.craft/templates/custom/`.
```bash
craft template save my_template
craft template delete my_template
craft template update my_template
craft template list
craft template list --lang c
craft template where my_template
```

### `craft config <subcommand>`
Reads and writes global defaults stored in `~/.craft/config.toml`. These defaults are used when creating new projects and can be overridden per-project in `craft.toml`.
```bash
craft config set defaults.lang cpp
craft config set defaults.cpp_standard 17
craft config get defaults.lang
craft config list
```

### `craft help [topic]`
Shows help for commands, subcommands, and `craft.toml`.
```bash
craft help
craft help build
craft help add
craft help template save
craft help config set
craft help craft.toml
```

### `craft upgrade`
Updates Craft itself to the latest version. Craft will check if the version on GitHub is newer than your local version and update it if so.
```bash
craft upgrade
```

---

## Built-in Templates

Craft ships with built-in templates for both C and C++:

| Template | Description |
|----------|-------------|
| `executable` | Standard executable project (default) |
| `static-library` | Static library project |
| `shared-library` | Shared library project |
| `header-only` | Header-only library |
```bash
craft project my_lib --template static-library --lang c
```

---

## craft.toml

`craft.toml` is the single source of truth for how your project is built. Craft reads it to generate `CMakeLists.txt` automatically — you never need to write or edit CMake directly.

### `[project]` fields

| Key | Required | Description |
|-----|----------|-------------|
| `name` | yes | Project name. Also used as the executable name. |
| `version` | yes | Version string in semver format e.g. `1.0.0` |
| `language` | yes | `c` or `cpp` |
| `c_standard` | no | C standard: `89`, `99`, `11`, `17`, `23` |
| `cpp_standard` | no | C++ standard: `11`, `14`, `17`, `20`, `23` |

### `[build]` fields

| Key | Required | Description |
|-----|----------|-------------|
| `type` | yes | `executable`, `static-library`, `shared-library`, or `header-only` |
| `include_dirs` | no | Directories added to the include path. Default: `["include"]` |
| `source_dirs` | no | Directories containing source files. Default: `["src"]` |
| `lib_dirs` | no | Directories to search for libraries |
| `libs` | no | Libraries to link against |

### `[dependencies]` fields

Dependencies are added and removed via `craft add` and `craft remove`. The `[dependencies]` section is managed by Craft — you rarely need to edit it manually.
```toml
[dependencies]
my_lib = { path = "../my_lib" }
raylib = { git = "https://github.com/raysan5/raylib", tag = "5.5" }
json = { git = "https://github.com/nlohmann/json", tag = "v3.11.2" }
SFML = { git = "https://github.com/SFML/SFML", tag = "3.0.0", links = ["SFML::Graphics", "SFML::Window", "SFML::System"] }
```

| Key | Description |
|-----|-------------|
| `path` | Path to a local Craft project |
| `git` | Git URL to clone |
| `tag` | Pin to a specific tag |
| `branch` | Pin to a specific branch |
| `links` | CMake link targets, required for non-Craft CMake projects |

> **Note:** `CMakeLists.txt` is generated automatically by Craft and should not be edited directly. Add a `CMakeLists.extra.cmake` file to your project root for any custom CMake — it will be included at the end of the generated file.

---

## Global Configuration

Global defaults live in `~/.craft/config.toml`:
```toml
[defaults]
language = "cpp"
c_standard = 99
cpp_standard = 17
template = "executable"
```

These are used when creating new projects. Project-level settings in `craft.toml` always take priority.

---

## Directory Structure
A typical Craft project looks like:
```
my_app/
├── craft.toml
├── CMakeLists.txt           # generated by Craft, do not edit
├── CMakeLists.extra.cmake   # optional, your custom CMake additions
├── build/
├── include/
├── src/
│   └── main.cpp
└── .craft/
    └── deps/                # cloned git dependencies
```

---

## Contributing

Craft is open source and community driven. Contributions are welcome — feel free to open issues or pull requests.
