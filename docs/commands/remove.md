# craft remove

Removes a dependency from the project.

## Usage

```bash
craft remove <name>
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `<name>` | yes | Name of the dependency to remove |

## Description

Removes a dependency from `craft.toml` and regenerates `CMakeLists.txt`. For git dependencies the cloned directory in `.craft/deps/` is also deleted.

- **Path dependencies** — removed from `craft.toml` only. The local directory is not touched.
- **Git dependencies** — removed from `craft.toml` and deleted from `.craft/deps/<name>/`.

## Examples

```bash
craft remove raylib
craft remove my_lib
craft remove physics_engine
```

## After Removing

Run `craft clean` and `craft build` to fully rebuild without the dependency:

```bash
craft clean
craft build
```

## See Also

- [craft add](add.md) — add a dependency
- [craft update](update.md) — update git dependencies