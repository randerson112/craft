# craft build

Builds the project in the current directory.

## Usage

```bash
craft build
```

## Description

Reads `craft.toml`, regenerates `CMakeLists.txt` if needed, fetches any missing git dependencies, and builds the project using CMake.

Craft walks up the directory tree to find the project root so you can run `craft build` from any subdirectory of your project.

### Build Steps

1. Locate `craft.toml` by walking up the directory tree
2. Load and validate `craft.toml`
3. Fetch any git dependencies that have not been cloned yet
4. Regenerate `CMakeLists.txt` if `craft.toml` is newer than the existing `CMakeLists.txt`
5. Configure with CMake
6. Build with CMake

### Dependency Caching

Git dependencies are built inside `.craft/deps/<name>/build/` rather than inside the project's `build/` directory. This means:

- Dependencies only need to be built once
- `craft clean` removes your project's build artifacts but leaves dependency builds intact
- Running `craft build` after `craft clean` only recompiles your project, not its dependencies

## Examples

```bash
# Build from project root
craft build

# Build from a subdirectory
cd src
craft build
```

## Output

```
Configuring...
Building...
Build successful
```

## See Also

- [craft run](run.md) — run the built executable
- [craft clean](clean.md) — remove build artifacts
- [craft add](add.md) — add dependencies