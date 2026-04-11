# craft project

Creates a new Craft project at the given path.

## Usage

```bash
craft project <path> [options]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `<path>` | yes | Path where the new project will be created |

## Options

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--lang <language>` | `-l` | Language to use: `c` or `cpp`. Defaults to global config. |
| `--template <name>` | `-t` | Template to use. Defaults to global config. |
| `--no-git` | | Skip git initialization |

## Description

Creates a new directory at the given path and initializes a Craft project inside it with the following structure:

```
my_app/
├── craft.toml
├── CMakeLists.txt
├── .gitignore
├── include/
├── src/
│   └── main.cpp or main.c
└── .craft/
    └── deps/
```

A git repository is initialized automatically. Use `--no-git` to skip this.

Use `craft init` instead if the directory already exists.

## Examples

```bash
# Create a C++ executable project
craft project my_app

# Create a C project
craft project my_app --lang c

# Create a static library
craft project my_lib --template static-library

# Create a project without git
craft project my_app --no-git

# Create from a custom template
craft project my_game --template my_game_template
```

## Built-in Templates

| Template | Description |
|----------|-------------|
| `executable` | Standard executable (default) |
| `static-library` | Static library |
| `shared-library` | Shared library |
| `header-only` | Header-only library |

## See Also

- [craft init](init.md) — initialize a project in an existing directory
- [craft template](template.md) — manage project templates
- [craft.toml reference](../craft-toml.md)