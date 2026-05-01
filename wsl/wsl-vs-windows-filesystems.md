# WSL and Windows Are Separate Systems
> **Tags:** `wsl`, `filesystem`, `windows`, `mnt`
> **Date:** 2026-04-30
> **Related:** [installing-go](../go/installing-go.md), [claude-code-install](../tools/claude-code-install.md)

WSL runs a full Linux system inside Windows — separate filesystems, separate PATH, separate installed programs.

- Installing Go in WSL does NOT make it available in Windows, and vice versa.
- WSL can access Windows files via `/mnt/c/Users/<username>/`, but this is file access only — not shared programs.
- WSL can run Windows `.exe` files (e.g., `notepad.exe`, `explorer.exe .`), but that's running the Windows binary through Windows, not a shared install.
- Cross-filesystem I/O (`/mnt/c/...`) is slow. Keep dev work on the Linux side: `~/projects/`, not `/mnt/c/Users/.../projects/`.

Mental model:

```
Windows:  C:\Program Files\...     → its own programs, its own Path
WSL:      /usr/local/bin/...       → its own programs, its own PATH
          ~/.local/bin/...

Shared:   /mnt/c/ (file access only, slow I/O)
```
