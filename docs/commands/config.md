# craft config

Manages global Craft configuration.

## Usage

```bash
craft config <subcommand> [args]
```

## Subcommands

| Subcommand | Description |
|------------|-------------|
| `set <key> <value>` | Set a global config value |
| `get <key>` | Get a global config value |
| `list` | Show all global config values |

---

## craft config set

Sets a global configuration value.

```bash
craft config set <key> <value>
```

```bash
craft config set defaults.language cpp
craft config set defaults.cpp_standard 23
craft config set defaults.c_standard 11
craft config set defaults.template static-library
```

---

## craft config get

Gets a global configuration value.

```bash
craft config get <key>
```

```bash
craft config get defaults.language
craft config get defaults.cpp_standard
```

---

## craft config list

Shows all global configuration values.

```bash
craft config list
```

Output:
```
[defaults]
language = "cpp"
c_standard = 99
cpp_standard = 17
template = "executable"
```

---

## Configuration Keys

| Key | Valid Values | Description |
|-----|-------------|-------------|
| `defaults.language` | `c`, `cpp` | Default language for new projects |
| `defaults.c_standard` | `89`, `99`, `11`, `17`, `23` | Default C standard |
| `defaults.cpp_standard` | `11`, `14`, `17`, `20`, `23` | Default C++ standard |
| `defaults.template` | any template name | Default template for new projects |

## Configuration File

Global configuration is stored in `~/.craft/config.toml`:

```toml
[defaults]
language = "cpp"
c_standard = 99
cpp_standard = 17
template = "executable"
```

You can edit this file directly if preferred.

## Notes

- Project-level settings in `craft.toml` always take priority over global config
- Global config is used as defaults when creating new projects with `craft project` or `craft init`

## See Also

- [craft project](project.md) — uses global config as defaults
- [craft init](init.md) — uses global config as defaults
- [craft.toml reference](../craft-toml.md)