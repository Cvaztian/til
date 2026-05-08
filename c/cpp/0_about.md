# About C++

C++ is not a different language from C — it is C with a large set of additions layered on top. Valid C is (almost) valid C++. The mental model from C still applies: memory, the stack, pointers, undefined behavior. What C++ adds is a way to manage that complexity at scale.

## Origins
Created by Bjarne Stroustrup at Bell Labs in 1983, originally called "C with Classes." The goal was to bring the object-oriented ideas from Simula into C without giving up C's performance or control. C++ has evolved significantly since — C++11 (2011) was a near-complete modernization, and C++17/20 continue to add high-level features while keeping zero-cost abstraction as the core promise.

## The C++ philosophy

**Zero-cost abstractions.** C++'s defining principle: abstractions you don't use cost nothing, and abstractions you do use are as fast as if you had written the equivalent C by hand. `std::vector` compiles to the same machine code as a manually managed `malloc`/`realloc` array. A class with no virtual methods has no runtime overhead over a C struct.

**You don't pay for what you don't use.** If you don't use exceptions, they add no overhead. If you don't use virtual dispatch, there's no vtable. C++ is opt-in to cost.

**Multi-paradigm.** C++ supports procedural (like C), object-oriented (classes, inheritance), and generic (templates) programming. You choose the right tool for each problem — they compose freely.

**RAII is the core idiom.** Resource Acquisition Is Initialization: tie resource lifetime to object lifetime. Constructors acquire, destructors release. The compiler guarantees destructors run when objects go out of scope — including on early returns and exceptions. This replaces the `goto cleanup` pattern from C entirely.

## What C++ overrides from C

| C pattern | C++ replacement | Why |
|-----------|----------------|-----|
| `NULL` | `nullptr` | Typed null — no accidental conversion to `int` |
| Raw pointers for ownership | `unique_ptr`, `shared_ptr` | Automatic, exception-safe memory management |
| `malloc`/`free` | `new`/`delete` (or smart pointers) | Constructors/destructors run correctly |
| `char[]` + `strcpy` | `std::string` | Safe, growable, no buffer overflows |
| Raw arrays + length | `std::vector`, `std::array` | Bounds-aware, self-describing |
| `goto cleanup` | RAII / destructors | Cleanup is automatic and exception-safe |
| `#define` constants | `constexpr` | Typed, scoped, debuggable |
| Naming collisions / prefixes | namespaces | Scoped names without prefixes |
| `void *` generics | templates | Type-safe generic code |
| Function pointers | lambdas + `std::function` | Closures with captured state |

## What C++ adds that C has no equivalent for
- **Classes** — encapsulation, constructors/destructors, operator overloading
- **Templates** — compile-time generic programming
- **References** — aliases that cannot be null and cannot be reseated
- **Exceptions** — structured error propagation (optional, often avoided in systems code)
- **The STL** — a large standard library of containers, algorithms, and utilities
- **Lambdas** — anonymous functions with captured state
- **`auto`** — type inference, essential with verbose template types

## Compilation

```bash
# C++ files use .cpp extension
# Use g++ (or clang++) instead of gcc

g++ -Wall -Wextra -std=c++17 -o program main.cpp

# With debug symbols
g++ -Wall -Wextra -std=c++17 -g -o program main.cpp

# Multiple files
g++ -Wall -Wextra -std=c++17 -o program main.cpp utils.cpp
```

Use `-std=c++17` as the default — it is widely supported and covers all modern idioms. C++20 adds more (concepts, ranges, coroutines) but compiler support varies.

## Conventions

- File extensions: `.cpp` for source, `.h` or `.hpp` for headers (`.hpp` signals C++-only)
- Naming: the STL uses `snake_case`; most C++ codebases follow suit; some use `PascalCase` for types
- Prefer `constexpr` over `#define` for constants
- Prefer `nullptr` over `NULL` or `0` for pointers
- Prefer smart pointers over raw owning pointers
- Prefer STL containers over raw arrays
- Use `const` on methods that don't mutate (`int get() const`)
- Mark single-argument constructors `explicit` to prevent implicit conversions

## Relation to C
C++ is a superset of C in practice, not by standard. A few C patterns are invalid C++ (implicit `void *` cast, `restrict`, VLAs in some contexts). But the mental model transfers completely — C++ does not hide the machine from you, it gives you better tools to manage it.
