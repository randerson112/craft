# craft run

Runs a compiled executable in a project or workspace.

## Usage

```bash
craft run [executable name]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `[executable name]` | no | Name of the executable to run. Defaults to the project name. Must specify name if workspace has multiple executables. |

## Description

Executes the compiled binary from the `build/` directory. With no arguments, looks for an executable named after the project as defined in `craft.toml`. If in a workspace, looks for executable named after only executable member or specified name if there are multiple executable members.

Can be run from any subdirectory. Craft walks up the directory tree to find the build directory.

## Examples

```bash
# Run the project executable
craft run

# Run a specific executable
craft run my_app
```

## See Also

- [craft workspace](workspace.md) - learn how workspaces function
- [craft build](build.md) - build the project before running