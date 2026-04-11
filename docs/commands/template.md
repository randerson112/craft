# craft template

Manages project templates.

## Usage

```bash
craft template <subcommand> [args] [options]
```

## Subcommands

| Subcommand | Description |
|------------|-------------|
| `save <name>` | Save the current project as a template |
| `delete <name>` | Delete a custom template |
| `update <name>` | Replace a template with the current project |
| `list` | List all available templates |
| `where <name>` | Show the path to a template on disk |

---

## craft template save

Saves the current project as a reusable template.

```bash
craft template save <name>
```

Snapshots the current project structure and saves it to `~/.craft/templates/custom/`. The template's `craft.toml` has its `name` and `version` stripped so they are set fresh on project creation.

Cannot overwrite a built-in template.

```bash
craft template save my_game_template
```

---

## craft template delete

Deletes a custom template.

```bash
craft template delete <name> [options]
```

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--lang <language>` | `-l` | Language of the template: `c` or `cpp` |

```bash
craft template delete my_game_template
craft template delete my_template --lang c
```

---

## craft template update

Replaces an existing template with the current project structure. Equivalent to `delete` followed by `save`.

```bash
craft template update <name>
```

```bash
craft template update my_game_template
```

---

## craft template list

Lists all available templates grouped by language.

```bash
craft template list [options]
```

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--lang <language>` | `-l` | Filter by language: `c` or `cpp` |
| `--all` | `-a` | Show all templates regardless of language |

```bash
craft template list
craft template list --lang c
craft template list --all
```

Output:
```
Built-in templates (cpp):
    executable
    static-library
    shared-library
    header-only

Built-in templates (c):
    executable
    static-library
    shared-library
    header-only

Custom templates (cpp):
    MyGameTemplate

Custom templates (c):
```

---

## craft template where

Shows the path to a template on disk.

```bash
craft template where <name> [options]
```

| Option | Shorthand | Description |
|--------|-----------|-------------|
| `--lang <language>` | `-l` | Language of the template: `c` or `cpp` |

```bash
craft template where my_game_template
# Output: /path/to/my/game/template
```

---

## Built-in Templates

| Template | Description |
|----------|-------------|
| `executable` | Standard executable project (default) |
| `static-library` | Static library project |
| `shared-library` | Shared library project |
| `header-only` | Header-only library |

## Template Storage

Templates are stored in `~/.craft/templates/`:

```
~/.craft/templates/
├── builtin/
│   ├── c/
│   │   ├── executable/
│   │   ├── static-library/
│   │   ├── shared-library/
│   │   └── header-only/
│   └── cpp/
│       ├── executable/
│       ├── static-library/
│       ├── shared-library/
│       └── header-only/
└── custom/
    ├── c/
    └── cpp/
```

## See Also

- [craft project](project.md) — create a project from a template
- [craft init](init.md) — initialize a project from a template