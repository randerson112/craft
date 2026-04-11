# craft upgrade

Updates Craft to the latest version.

## Usage

```bash
craft upgrade
```

## Description

Checks GitHub for the latest version of Craft and updates if a newer version is available. Only the Craft binary is replaced. Your configuration, templates, and project dependencies in `~/.craft/` are never touched.

## Output

**Update available:**
```
Checking for updates...
Current version: 0.8.0
Latest version:  1.0.0
Updating Craft...
Updated Craft to 1.0.0 successfully
Restart your terminal to use the new version
```

**Already up to date:**
```
Checking for updates...
Craft is already up to date (1.0.0)
```

## Notes

- Requires an internet connection to check for updates
- Restart your terminal after updating to use the new version
- Run `craft --version` to verify the update was successful