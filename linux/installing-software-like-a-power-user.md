# How to Install Software on Linux — The Power User Hierarchy
> **Tags:** `apt`, `package-manager`, `install`, `snap`, `brew`
> **Date:** 2026-04-30
> **Related:** [installing-go](../go/installing-go.md), [claude-code-install](../tools/claude-code-install.md)

Search/install priority, top to bottom:

1. **`apt search` / `apt install`** — your distro's package manager. First instinct. Limited to what Ubuntu/Debian packages (often older versions).
2. **`snap find` / `snap install`** — Canonical's curated store. Fresher than apt for some tools.
3. **`brew search` / `brew install`** — Homebrew works on Linux too. Good for dev tools.
4. **Official tarball or install script** — when you need the exact latest version. This is what Go and Claude Code recommend.
5. **Version managers** (`pyenv`, `nvm`, `goenv`) — for juggling multiple versions of a language.

Before installing anything:

```bash
which <tool>        # do I already have it?
<tool> --version    # what version?
apt search <tool>   # is it in my repos?
```

`command-not-found` — Ubuntu's built-in handler. Type a tool name you don't have, and it suggests the package.

Distro repos prioritize stability over freshness. For system tools (curl, git, tmux), apt is fine. For language toolchains you're actively developing with, use the official source.
