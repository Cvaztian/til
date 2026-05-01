# How Linux Finds Commands — The PATH Variable
> **Tags:** `path`, `environment`, `shell`, `binaries`
> **Date:** 2026-04-30
> **Related:** [bashrc-and-source](bashrc-and-source.md), [claude-code-install](../tools/claude-code-install.md)

When you type a command (e.g. `go`, `claude`, `ls`), the shell searches directories listed in the `$PATH` environment variable, left to right, until it finds a matching binary.

```bash
echo $PATH                       # show the current list (colon-separated)
echo $PATH | tr ':' '\n'         # one directory per line — readable
which go                         # which binary did the shell resolve to?
command -v go && go version || echo "not installed"   # safe existence check
```

PATH is just a string variable:

```bash
export PATH="$HOME/.local/bin:$PATH"
```

The `$PATH` on the right reads the old value; the `=` assigns the new combined string back.

- **Prepending** (your dir first) → your version wins over system defaults
- **Appending** (your dir last) → system defaults win

The C equivalent:

```c
sprintf(new_path, "%s/.local/bin:%s", getenv("HOME"), getenv("PATH"));
setenv("PATH", new_path, 1);
```
