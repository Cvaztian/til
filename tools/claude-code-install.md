# Installing Claude Code on Linux / WSL
> **Tags:** `claude-code`, `install`, `curl`, `path`
> **Date:** 2026-04-30
> **Related:** [how-linux-finds-commands](../linux/how-linux-finds-commands.md), [bashrc-and-source](../linux/bashrc-and-source.md), [wsl-vs-windows-filesystems](../wsl/wsl-vs-windows-filesystems.md)

Install via the native installer (see [claude.ai/download](https://claude.ai/download) for the current command).

The installer drops the binary in `~/.local/bin/claude`. Adding `~/.local/bin` to PATH is a one-time addition — any future tool that installs there will also just work, no extra PATH edits needed.

On the `curl -fsSL` flags you'll see in most install scripts:
- `-f` — fail silently on HTTP errors (no junk output on 404)
- `-s` — suppress progress bar
- `-S` — but still show actual errors
- `-L` — follow redirects

Requires: Claude Pro, Max, Team, Enterprise, or Console account.
