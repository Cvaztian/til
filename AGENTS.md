# Agent Instructions

Rules for maintaining this repo. Read this before adding or editing any entry.

## File naming
Lowercase, kebab-case: `how-linux-finds-commands.md`. Names should be greppable and self-descriptive.

## Entry frontmatter
Every entry starts with:
```
# Title Here
> **Tags:** `tag1`, `tag2`
> **Date:** YYYY-MM-DD
> **Related:** [other-entry](../category/other-entry.md)
```
Tags enable cross-cutting search: `grep -r "shell" til/` finds everything tagged `shell` across all categories.

## Category directories
Each category gets its own directory with a `README.md` listing its entries.

Current: `linux/`, `go/`, `tools/`, `wsl/`

Future (create as needed): `git/`, `docker/`, `networking/`, `devops/`, `security/`, `databases/`, `testing/`, `python/`, `c/`, `typescript/`

## Adding a new entry
1. Create the `.md` file in the appropriate category directory using `_templates/entry.md`
2. Update that category's `README.md`
3. Update the root `README.md` index (entry count, stats line, table row)
4. Commit: `til: <category> — <short description>`
