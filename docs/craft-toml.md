# craft.toml Reference

`craft.toml` is the project configuration file for Craft. It is the single source of truth for how your project is built. Craft reads it to generate `CMakeLists.txt` automatically.

`craft.toml` is generated when you run `craft project` or `craft init`. You can edit it manually at any time. Craft will regenerate `CMakeLists.txt` automatically on the next `craft build` if `craft.toml` has been modified.

---

## Sections

- [`[project]`](#project) — project metadata
- [`[build]`](#build) — build configuration
- [`[dependencies]`](#dependencies) — project dependencies

---

## [project]

Defines the project metadata.

```toml
[project]
name = "my_app"
version = "0.1.0"
language = "cpp"
cpp_standard = 17
```

### Fields

| Field | Required | Type | Description |
|-------|----------|------|-------------|
| `name` | yes | string | Name of the project. Also used as the executable or library name. |
| `version` | yes | string | Version in semver format `1.2.3` |
| `language` | yes | string | `c` or `cpp` |
| `c_standard` | no | integer | C standard: `89`, `99`, `11`, `17`, `23` |
| `cpp_standard` | no | integer | C++ standard: `11`, `14`, `17`, `20`, `23` |

### Notes

- `name` must be a valid CMake target name. Alphanumeric characters, underscores, and hyphens only
- `version` must follow semver format: `MAJOR.MINOR.PATCH`
- If no standard is specified CMake will use the compiler default
- `VERSION` is automatically embedded into your binary as a preprocessor macro so you can access the version at runtime:

```c
printf("my_app v%s\n", VERSION);
```

---

## [build]

Defines how the project is built.

```toml
[build]
type = "executable"
include_dirs = ["include"]
source_dirs = ["src"]
lib_dirs = ["lib"]
libs = ["mylib"]
```

### Fields

| Field | Required | Type | Description |
|-------|----------|------|-------------|
| `type` | yes | string | Build type. See [Build Types](#build-types) |
| `include_dirs` | no | array of strings | Directories added to the include path. Default: `["include"]` |
| `source_dirs` | no | array of strings | Directories containing source files. Default: `["src"]` |
| `lib_dirs` | no | array of strings | Directories to search for pre-built libraries |
| `libs` | no | array of strings | Pre-built libraries to link against |

### Build Types

| Type | Description | CMake Target |
|------|-------------|--------------|
| `executable` | Standard executable (default) | `add_executable` |
| `static-library` | Static library `.a` / `.lib` | `add_library STATIC` |
| `shared-library` | Shared library `.dylib` / `.so` / `.dll` | `add_library SHARED` |
| `header-only` | Header-only library | `add_library INTERFACE` |

### Notes

- Source files are discovered recursively inside `source_dirs` using `GLOB_RECURSE`
- Source files in the project root directory are also included automatically
- `lib_dirs` and `libs` are for pre-built libraries already on disk. For git or path dependencies use the [`[dependencies]`](#dependencies) section instead
- `CMakeLists.txt` is regenerated automatically whenever `craft.toml` is newer than the existing `CMakeLists.txt`

### Examples

**Executable with multiple source directories:**
```toml
[build]
type = "executable"
include_dirs = ["include", "vendor/include"]
source_dirs = ["src", "platform"]
```

**Static library:**
```toml
[build]
type = "static-library"
include_dirs = ["include"]
source_dirs = ["src"]
```

**Linking against a pre-built library:**
```toml
[build]
type = "executable"
lib_dirs = ["lib/mylib"]
libs = ["mylib"]
```

---

## [dependencies]

Defines project dependencies. Dependencies are managed via `craft add` and `craft remove`. You rarely need to edit this section manually.

```toml
[dependencies]
my_lib = { path = "../my_lib" }
raylib = { git = "https://github.com/raysan5/raylib.git", tag = "5.5", links = ["raylib"] }
json = { git = "https://github.com/nlohmann/json.git"}
```

### Dependency Types

#### Path Dependency

A local Craft project on your machine.

```toml
my_lib = { path = "../my_lib" }
```

| Field | Required | Description |
|-------|----------|-------------|
| `path` | yes | Relative path to the local Craft project |

- The dependency must be a valid Craft project with its own `craft.toml`
- The dependency must not be an `executable` type, only libraries can be dependencies
- Craft reads the dependency's `craft.toml` to determine its include directories automatically
- Add with: `craft add --path ../my_lib`

#### Git Dependency

A dependency fetched from a git repository.

```toml
raylib = { git = "https://github.com/raysan5/raylib.git", links = ["raylib"] }
```

| Field | Required | Description |
|-------|----------|-------------|
| `git` | yes | Git repository URL |
| `tag` | no | Pin to a specific tag. Recommended for reproducible builds |
| `branch` | no | Pin to a specific branch |
| `links` | no | CMake link targets. Required for non-Craft CMake projects |

- `tag` and `branch` are mutually exclusive
- If neither `tag` nor `branch` is specified the latest commit is used
- If the git dependency is a Craft project `links` is not needed, Craft figures it out automatically
- If the git dependency is a non-Craft CMake project you must specify `links` with the correct CMake target names
- Dependencies are cloned to `.craft/deps/<name>/` and built there
- Add with: `craft add --git <url> --links <links>`

#### Registry Kit

A package from the Craft registry. Registry kits are just git dependencies with their metadata stored in the registry.

```toml
raylib = { git = "https://github.com/raysan5/raylib.git", links = ["raylib"] }
```

Registry kits are written to `craft.toml` as regular git dependencies after being resolved from the registry. Add with: `craft add raylib`

### Notes

- `craft build` automatically fetches any git dependencies that are not yet cloned
- Use `craft update` to re-fetch git dependencies that are not pinned to a tag
- Use `craft remove <name>` to remove a dependency

### Examples

**A project with multiple dependency types:**
```toml
[dependencies]
# Local Craft library
physics_engine = { path = "../physics_engine" }

# Registry kit
raylib = { git = "https://github.com/raysan5/raylib.git", tag = "5.5", links = ["raylib"] }

# Non-Craft CMake project requiring explicit link targets
SFML = { git = "https://github.com/SFML/SFML.git", links = ["SFML::Graphics", "SFML::Window", "SFML::System"] }

# Header-only library, no links needed
json = { git = "https://github.com/nlohmann/json.git", tag = "v3.11.2" }
```

---

## Custom CMake

For anything beyond what `craft.toml` currently supports, add a `CMakeLists.extra.cmake` file to your project root. Craft will include it in the generated `CMakeLists.txt` automatically.

`CMakeLists.extra.cmake` should be committed to your repository. `CMakeLists.txt` should be gitignored since it is generated by Craft.

---

## Full Example

```toml
[project]
name = "FlappyBird"
version = "1.0.0"
language = "cpp"
cpp_standard = 17

[build]
type = "executable"
include_dirs = ["include"]
source_dirs = ["src"]

[dependencies]
physics_engine = { path = "../physics_engine" }
raylib = { git = "https://github.com/raysan5/raylib.git", tag = "5.5", links = ["raylib"] }
```

---

## Unknown Keys

Craft will warn you if `craft.toml` contains unknown keys in any section. This helps catch typos before they silently have no effect:

```
Warning: Unknown key 'soruce_dirs' in [build], did you mean 'source_dirs'?
```