# craft help

Shows help for Craft commands and topics.

## Usage

```bash
craft help
craft help [command] [subcommand]
```

## Arguments

| Argument | Required | Description |
|----------|----------|-------------|
| `<command>` | no | Command to get help for |
| `<subcommand>` | no | Subcommand to get help for |

## Examples

```bash
# Show general help
craft help

# Show help for a command
craft help build
craft help add
craft help template

# Show help for a subcommand
craft help template save
craft help config set

# Show craft.toml reference
craft help craft.toml
```

## Available Topics

All commands:
- `project`, `init`, `build`, `run`, `gen`, `clean`
- `add`, `remove`, `update`
- `template`, `config`, `upgrade`
- `help`

Special topics:
- `craft.toml` — project configuration reference

## See Also

- [craft.toml reference](../craft-toml.md) — full configuration reference
- [Getting Started](../getting-started.md) — quick start guide