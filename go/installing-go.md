# Installing Go on Linux (the right way)
> **Tags:** `go`, `install`, `tarball`, `path`
> **Date:** 2026-04-30
> **Related:** [how-linux-finds-commands](../linux/how-linux-finds-commands.md), [installing-software-like-a-power-user](../linux/installing-software-like-a-power-user.md)

Don't use `apt install golang` — it ships old versions. Use the official tarball from [go.dev/dl](https://go.dev/dl).

Two directories to understand once it's installed:
- `/usr/local/go/bin` — the Go toolchain itself (the `go` command)
- `$HOME/go/bin` — where `go install` puts binaries you compile or fetch later (linters, CLI tools, your own programs)

Both need to be on your PATH.
