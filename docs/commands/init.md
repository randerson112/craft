# craft init

Initializes a Craft project in the current or specified directory.

## Usage

```bash
craft init [path] [options]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `[path]` | no | Path to initialize. Defaults to current directory. |

## Options

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--lang <language>` | `-l` | Language override: `c` or `cpp` |
| `--template <n>` | `-t` | Template to use (empty directories only) |
| `--no-git` | | Skip git initialization |

## Description

Behavior depends on what is already in the directory:

### Empty Directory

Generates a full project structure including `craft.toml`, `CMakeLists.txt`, `src/`, `include/`, and a starter main file using the specified or default template. Identical to `craft project` except it does not create the directory.

### Existing Project

Scans the directory to detect:
- **Language** — from file extensions (`.cpp`, `.cc`, `.cxx`, `.c`). Majority wins.
- **Source directories** — directories containing source files
- **Include directories** — directories containing header files
- **Libraries** — directories containing `.a`, `.dylib`, `.so`, `.lib`, or `.dll` files

Generates `craft.toml` and `CMakeLists.txt` from what was found. Existing source files are never modified.

If a `CMakeLists.txt` already exists it is backed up to `CMakeLists.backup.cmake` before being replaced.

## Examples

```bash
# Initialize in current directory
craft init

# Initialize in a specific directory
craft init my_app

# Force C language for existing project
craft init --lang c

# Use a template for an empty directory
craft init --template static-library

# Skip git initialization
craft init --no-git
```

## Migrating an Existing Project

```bash
cd my_existing_project
craft init
```

After running `craft init` on an existing project:

1. Open `craft.toml` and verify the detected settings are correct
2. Check the build type. Craft defaults to `executable`, change it if needed
3. If `CMakeLists.backup.cmake` was created, migrate any custom CMake into `CMakeLists.extra.cmake`
4. Run `craft add` to add your dependencies
5. Run `craft build` to verify everything works

## See Also

- [craft project](project.md) — create a new project directory
- [craft add](add.md) — add dependencies after init
- [craft.toml reference](../craft-toml.md)