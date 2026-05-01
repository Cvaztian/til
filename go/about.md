# About Go

## Origins
Created at Google in 2007 by Rob Pike, Ken Thompson (co-creator of Unix and C), and Robert Griesemer. Open-sourced in 2009. Designed as a reaction to the pain of working with C++ at scale — they wanted C-level performance with Python-level simplicity and built-in concurrency.

## What kind of language is it?
- **Compiled** to native machine code — produces a single static binary with no runtime dependencies
- **Statically typed** — types are checked at compile time, like C
- **Garbage collected** — memory is managed automatically, unlike C
- **Concurrency-first** — goroutines and channels are built into the language, not bolted on

The single binary output is a big deal for deployment: you build once, copy the file anywhere, and it runs. No `npm install`, no virtual environments, no JVM.

## Who uses it and for what?
Go dominates **cloud infrastructure and backend tooling**:
- Docker, Kubernetes, Terraform, Prometheus — all written in Go
- Cloudflare, Dropbox, Uber, Stripe use it for backend services
- GitHub, HashiCorp, and most of the cloud-native ecosystem

Common use cases: CLIs, microservices, APIs, infrastructure tooling, anything that needs to be fast and deployable as a single binary.

## Context today (2026)
Go is the default language of cloud-native infrastructure. If a tool is part of the Kubernetes/DevOps ecosystem, it's almost certainly Go. The language has matured significantly — generics were added in 1.18 (2022), the standard library keeps improving, and WebAssembly support is growing for edge computing use cases.

Competitive pressure from Rust for systems-level work, but Go remains the pragmatic choice when you need developer velocity alongside solid performance.

## Pros
- Extremely fast compile times
- Simple, consistent syntax — small language surface area
- Excellent standard library (HTTP server, JSON, crypto, all built-in)
- Single binary deployment
- Built-in tooling: formatter (`gofmt`), test runner, race detector
- Strong backwards compatibility guarantee

## Cons
- Error handling is verbose — `if err != nil` repeated everywhere
- Generics arrived late (1.18) and the ecosystem is still catching up
- No default parameter values or function overloading
- Garbage collector adds latency (not suitable for hard real-time systems)
- Smaller package ecosystem than Python or Node

## Recommendations
Use Go when you're building: a CLI tool, a backend API, an infrastructure component, or anything that needs to run as a single binary on a server. It's the right default for backend/DevOps work.

Reach for Rust instead if you need zero-cost abstractions and manual memory control (embedded, OS-level, performance-critical hot paths). Reach for Python if iteration speed matters more than runtime performance.
