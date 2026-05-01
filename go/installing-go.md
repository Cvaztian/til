# Installing Go on Linux (the right way)
> **Tags:** `go`, `install`, `tarball`, `path`
> **Date:** 2026-04-30
> **Related:** [how-linux-finds-commands](../linux/how-linux-finds-commands.md), [installing-software-like-a-power-user](../linux/installing-software-like-a-power-user.md)

Don't use `apt install golang` — it ships older versions. Use the official tarball.

```bash
# Download latest (check https://go.dev/dl for current version)
curl -OL https://go.dev/dl/go1.26.2.linux-amd64.tar.gz

# Remove old install and extract
sudo rm -rf /usr/local/go
sudo tar -C /usr/local -xzf go1.26.2.linux-amd64.tar.gz

# Add to PATH (append to ~/.bashrc)
echo 'export PATH=$PATH:/usr/local/go/bin:$HOME/go/bin' >> ~/.bashrc
source ~/.bashrc

# Verify
go version
```

Two directories to understand:
- `/usr/local/go/bin` — where the Go toolchain lives (the `go` command itself)
- `$HOME/go/bin` — where `go install` puts binaries you compile or install later

Latest stable version as of April 2026: Go 1.26.2.
