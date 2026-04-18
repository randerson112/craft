# craft build

Builds the local project or workspace.

## Usage

```bash
craft build
```

## Description

Reads `craft.toml`, regenerates `CMakeLists.txt` if needed, fetches any missing git dependencies, and builds the project or workspace using CMake.

Craft walks up the directory tree to find the root so you can run `craft build` from any subdirectory.

### Build Steps

1. Locate `craft.toml` by walking up the directory tree
2. Load and validate `craft.toml`
3. Fetch any git dependencies that have not been cloned yet
4. Regenerate `CMakeLists.txt` if `craft.toml` is newer than the existing `CMakeLists.txt`
5. Configure with CMake
6. Build with CMake

### Dependency Caching

Git dependencies are built inside `.craft/deps/<name>/build/` rather than inside the `build/` directory. This means:

- Dependencies only need to be built once
- `craft clean` removes the build artifacts but leaves dependency builds intact
- Running `craft build` after `craft clean` only recompiles your project or workspace, not its dependencies

## Examples

```bash
# Build from root
craft build

# Build from a subdirectory
cd src
craft build
```

## See Also

- [craft workspace](workspace.md) - managing a workspace
- [craft run](run.md) — run the built executable
- [craft clean](clean.md) — remove build artifacts
- [craft add](add.md) — add dependencies