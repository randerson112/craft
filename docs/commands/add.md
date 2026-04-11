# craft add

Adds a dependency to the project.

## Usage

```bash
craft add <kit>                          # registry kit
craft add --path <path>                  # local Craft project
craft add --git <url> [options]          # git dependency
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `<kit>` | no | Name of a kit from the Craft registry |

## Options

| Option | Description |
|--------|-------------|
| `--path <path>` | Add a local Craft project as a dependency |
| `--git <url>` | Add a git dependency by URL |
| `--tag <tag>` | Pin to a specific git tag (git only) |
| `--branch <branch>` | Pin to a specific git branch (git only) |
| `--links <targets>` | Comma separated CMake link targets (git only) |

## Description

Adds a dependency to `craft.toml` and regenerates `CMakeLists.txt` immediately so the project is always in a consistent state.

### Dependency Types

#### Registry Kit

Looks up the kit in the Craft registry, fetches the metadata, and adds it as a git dependency.

```bash
craft add raylib
craft add raylib --tag 5.0    # override the default tag
```

#### Local Path Dependency

Adds a local Craft project as a dependency. The path must point to a valid Craft project with a `craft.toml`. The dependency must be a library type.

```bash
craft add --path ../my_lib
craft add --path ../physics_engine
```

Craft reads the dependency's `craft.toml` to determine include directories automatically.

#### Git Dependency

Clones the repository to `.craft/deps/<name>/` and adds it as a dependency.

```bash
# Remote Craft project
craft add --git https://github.com/user/mylib.git

# Non-Craft CMake project, --links required
craft add --git https://github.com/SFML/SFML.git --tag 3.0.0 --links SFML::Graphics,SFML::Window,SFML::System

# Pinned to a branch
craft add --git https://github.com/user/mylib.git --branch develop
```

If the git repository is a Craft project, `--links` is not needed. Craft reads the dependency's `craft.toml` automatically.

If the git repository is a non-Craft CMake project, `--links` must specify the correct CMake target names. These are defined by the library and documented on its GitHub page.

### After Adding a Dependency

Run `craft build` to fetch and compile the new dependency:

```bash
craft build
```

## See Also

- [craft remove](remove.md) — remove a dependency
- [craft update](update.md) — update git dependencies
- [Craft Registry](https://github.com/randerson112/craft-registry) — browse available kits
- [craft.toml dependencies reference](../craft-toml.md#dependencies)