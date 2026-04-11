# craft update

Updates git dependencies to their latest version.

## Usage

```bash
craft update [name]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `[name]` | no | Name of a specific dependency to update. If omitted all dependencies are updated. |

## Description

Re-clones git dependencies to get the latest version. The behavior depends on how the dependency is pinned:

- **Pinned to a tag** — skipped. Tags point to a fixed commit so there is nothing to update.
- **Pinned to a branch** — re-cloned to get the latest commits on that branch.
- **No tag or branch** — re-cloned to get the latest commit.
- **Path dependencies** — always skipped. They always use the local version.

After updating run `craft build` to rebuild with the new versions.

## Examples

```bash
# Update all dependencies
craft update

# Update a specific dependency
craft update raylib

# Update a dependency pinned to a branch
craft update mylib
```

## Notes

- To update a tag-pinned dependency, manually edit the `tag` field in `craft.toml` and run `craft build`
- `craft update` does not modify `craft.toml`, it only re-clones the dependency source

## See Also

- [craft add](add.md) — add a dependency
- [craft remove](remove.md) — remove a dependency