# TIL — Today I Learned

Short notes on tools, languages, and engineering tricks I pick up along the way.

**Sebastián Calderón Yock** — Computer Engineer, 4+ years at Intel (device modeling, x86 microarchitectures, PCIe, AI SoC platforms).
Currently learning: Go, Linux power-user workflows, backend/infra tooling.
GitHub: [github.com/Cvaztian](https://github.com/Cvaztian)

**6 entries** across **4 categories**

---

## Index

### Linux (3 entries)
| Entry | Tags |
|-------|------|
| [How Linux Finds Commands](linux/how-linux-finds-commands.md) | `path`, `environment`, `shell`, `binaries` |
| [Installing Software Like a Power User](linux/installing-software-like-a-power-user.md) | `apt`, `package-manager`, `install`, `snap`, `brew` |
| [.bashrc and source](linux/bashrc-and-source.md) | `shell`, `config`, `environment`, `bashrc` |

### Go (1 entry)
| Entry | Tags |
|-------|------|
| [Installing Go on Linux](go/installing-go.md) | `go`, `install`, `tarball`, `path` |

### Tools (1 entry)
| Entry | Tags |
|-------|------|
| [Installing Claude Code](tools/claude-code-install.md) | `claude-code`, `install`, `curl`, `path` |

### WSL (1 entry)
| Entry | Tags |
|-------|------|
| [WSL and Windows Are Separate Systems](wsl/wsl-vs-windows-filesystems.md) | `wsl`, `filesystem`, `windows`, `mnt` |

---

## Conventions

### File naming
Lowercase, kebab-case: `how-linux-finds-commands.md`. Names should be greppable and self-descriptive.

### Entry frontmatter
Every entry starts with:
```
# Title Here
> **Tags:** `tag1`, `tag2`
> **Date:** YYYY-MM-DD
> **Related:** [other-entry](../category/other-entry.md)
```
Tags enable cross-cutting search: `grep -r "shell" til/` finds everything tagged `shell` across all categories.

### Category directories
Each category gets its own directory with a `README.md` listing its entries.

Current: `linux/`, `go/`, `tools/`, `wsl/`

Future (create as needed): `git/`, `docker/`, `networking/`, `devops/`, `security/`, `databases/`, `testing/`, `python/`, `c/`, `typescript/`

### Adding a new entry
1. Create the `.md` file in the appropriate category directory using `_templates/entry.md`
2. Update that category's `README.md`
3. Update this index (entry count, stats line, table row)
4. Commit: `til: <category> — <short description>`
