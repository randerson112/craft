# craft run

Runs the compiled project executable.

## Usage

```bash
craft run [executable]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `[executable]` | no | Name or path of the executable to run. Defaults to the project name. |

## Description

Executes the compiled binary from the `build/` directory. With no arguments, looks for an executable named after the project as defined in `craft.toml`.

Can be run from any subdirectory of the project. Craft walks up the directory tree to find the project root.

## Examples

```bash
# Run the project executable
craft run

# Run a specific executable
craft run my_app

# Run an executable outside the project
craft run ../some/other/binary
```

## See Also

- [craft build](build.md) — build the project before running