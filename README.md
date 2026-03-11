# Craft

A lightweight build tool for C and C++ projects — think Cargo, but for C/C++.

Craft takes the friction out of C/C++ development by managing your project structure, generating CMake configuration from a simple `craft.toml` file, and providing a clean command-line interface for common tasks.

---

## Quick Start

```bash
# Create a new project
craft project MyApp

# Build it
cd MyApp
craft build

# Run it
craft run
```

It's that easy! No more confusing CMake or Make setup.

---

## Commands

### `craft project <path>`
Creates a new project directory at the given path with a full project structure including `craft.toml`, `src/`, `include/`, and a starter main file.

```bash
craft project MyApp
craft project MyApp --lang c
craft project MyLib --template static-library
```

### `craft init [path]`
Initializes a Craft project inside an existing directory. Defaults to the current directory if no path is given.

```bash
craft init
craft init MyApp --lang cpp
```

### `craft build`
Reads `craft.toml`, regenerates `CMakeLists.txt` if needed, and builds the project using CMake. Can be run from any subdirectory — Craft walks up the directory tree to find the project root.

```bash
craft build
```

### `craft run [executable]`
Runs the project executable. Defaults to the project name from `craft.toml`. Optionally accepts an executable name to run any binary.

```bash
craft run
craft run my_other_binary
```

### `craft compile <source> [output]`
Compiles a single `.c` or `.cpp` file without a full project structure. Useful for quick one-off scripts.

```bash
craft compile main.cpp
craft compile main.cpp app
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

### `craft template <subcommand>`
Manages reusable project templates. Craft ships with built-in templates for common project types. Custom templates are saved to `~/.craft/templates/custom/`.

```bash
craft template save MyTemplate       # Save current project as a template
craft template delete MyTemplate     # Delete a custom template
craft template update MyTemplate     # Re-snapshot current project into template
craft template list                  # List all available templates
craft template list --lang c         # Filter by language
craft template where MyTemplate      # Show path to template on disk
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
craft help template save
craft help config set
craft help craft.toml
```

---

## Options

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--template <name>` | `-t` | Template to use when creating a project |
| `--lang <language>` | `-l` | Language to use: `c` or `cpp` |

---

## craft.toml

`craft.toml` is the project configuration file. It is the single source of truth for how your project is built. Craft reads it to generate `CMakeLists.txt` automatically.

```toml
[project]
name = "MyApp"
version = "0.1.0"
language = "cpp"
cpp_standard = 17

[build]
type = "executable"
include_dirs = ["include"]
source_dirs = ["src"]
```

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

> **Note:** `CMakeLists.txt` is generated automatically by Craft and should not be edited directly. Add a `CMakeLists.extra.cmake` file to your project root for any custom CMake beyond what Craft generates — it will be included at the end of the generated file.

---

## Built-in Templates

Craft ships with built-in templates for both C and C++:

| Template | Description |
|----------|-------------|
| `executable` | Standard executable project (default) |
| `static-library` | Static library project |
| `shared-library` | Shared library project |
| `header-only` | Header-only library |

Use `--template` when creating a project to select one:

```bash
craft project MyLib --template static-library --lang c
```

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

Craft uses `~/.craft/` to store its configuration and templates:

```
~/.craft/
├── bin/craft
├── config.toml
└── templates/
    ├── builtin/
    │   ├── c/
    │   │   ├── executable/
    │   │   ├── static-library/
    │   │   ├── shared-library/
    │   │   └── header-only/
    │   └── cpp/
    │       ├── executable/
    │       ├── static-library/
    │       ├── shared-library/
    │       └── header-only/
    └── custom/
        ├── c/
        └── cpp/
```

A typical Craft project looks like:

```
MyApp/
├── craft.toml
├── CMakeLists.txt          # generated by Craft, do not edit
├── CMakeLists.extra.cmake  # optional, your custom CMake additions
├── build/
├── include/
└── src/
    └── main.cpp
```

---

## Roadmap

- [x] v0.1.0 — Core commands: `project`, `init`, `build`, `run`, `compile`, `gen`, `clean`
- [x] v0.2.0 — `craft clean` with file count and byte reporting
- [x] v0.3.0 — Template system with builtin and custom templates
- [x] v0.4.0 — `craft.toml` parsing, global config, `craft config` command
- [x] v0.5.0 — CMake generation from `craft.toml`, improved error messages, project-aware `gen`

---

## Contributing

Craft is open source and community driven. Contributions are welcome — feel free to open issues or pull requests.

---

## License

MIT