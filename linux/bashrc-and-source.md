# What .bashrc Does and Why You `source` It
> **Tags:** `shell`, `config`, `environment`, `bashrc`
> **Date:** 2026-04-30
> **Related:** [how-linux-finds-commands](how-linux-finds-commands.md)

`~/.bashrc` runs automatically every time you open a new terminal. It sets up your personal environment: PATH additions, aliases, environment variables.

When you modify `.bashrc`, your current terminal doesn't know — it already loaded the old version at startup.

```bash
source ~/.bashrc    # re-execute the file in your current shell right now
```

Key operators:
- `>>` appends to a file. `>` overwrites. Never `>` your bashrc.
- `export` makes a variable available to child processes. Without it, only the shell itself sees it.
- `&&` chains commands: "run the second only if the first succeeds."

Mental model:

```
Terminal opens → runs ~/.bashrc → sets up environment
You edit ~/.bashrc → current terminal doesn't know
source ~/.bashrc → "re-read that file right now"
```
