# Go Modules and Packages
> **Tags:** `go`, `modules`, `packages`, `dependency-management`, `fundamentals`
> **Date:** 2026-04-30
> **Related:** [error-handling](error-handling.md)

## Packages
A package is a directory of `.go` files that share a `package <name>` declaration. It's the unit of code organization and reuse — equivalent to a C header + implementation, but with the namespacing built in.

Every identifier from another package is accessed as `package.Identifier`:

```go
import "fmt"
fmt.Println("hello")  // fmt is the package, Println is the function
```

Capitalized names are exported (public). Lowercase names are private to the package. That's the entire visibility system — no `public`/`private` keywords.

## Modules
A module is a collection of packages with a `go.mod` file at the root. It's your project.

```bash
go mod init github.com/Cvaztian/devstatus
```

The module name is conventionally the GitHub path — not a URL Go fetches, just a globally unique identifier so your package names don't collide with anyone else's.

`go.mod` is your project descriptor. `go.sum` is the lockfile — cryptographic hashes of every dependency so builds are reproducible.

## Key commands

```bash
go mod init <module-name>   # initialize a new module
go get <package>            # add a dependency
go mod tidy                 # sync go.mod/go.sum with actual imports
go run main.go              # compile and run (dev)
go build                    # compile to binary (ship)
```

`go mod tidy` is the one to remember — run it whenever imports and go.mod get out of sync.

## internal/ directory
Packages under `internal/` can only be imported by code in the same module. Use it to keep implementation details private from outside consumers:

```
devstatus/
├── main.go
└── internal/
    └── stats/
        └── stats.go        # importable as .../devstatus/internal/stats
                            # but not by any external project
```
