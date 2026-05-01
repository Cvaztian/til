# Installing Claude Code on Linux / WSL
> **Tags:** `claude-code`, `install`, `curl`, `path`
> **Date:** 2026-04-30
> **Related:** [how-linux-finds-commands](../linux/how-linux-finds-commands.md), [bashrc-and-source](../linux/bashrc-and-source.md)

Native installer (recommended, no dependencies):

```bash
curl -fsSL https://claude.ai/install.sh | bash
```

The `curl` flags: `-f` fail silently, `-s` no progress bar, `-S` show errors, `-L` follow redirects.

The installer puts the binary in `~/.local/bin/claude`. You may need to add that directory to PATH:

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc && source ~/.bashrc
```

This is a one-time PATH addition — any future tool installing to `~/.local/bin` will also just work.

Alternative via npm (requires Node.js 18+):

```bash
npm install -g @anthropic-ai/claude-code
```

Requires: Claude Pro, Max, Team, Enterprise, or Console account. Free plan does not include Claude Code.

On WSL: install and run from the WSL terminal, not PowerShell/CMD.

Verify: `claude --version`
