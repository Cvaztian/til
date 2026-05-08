# About C

## Origins
Created at Bell Labs in 1972 by Dennis Ritchie, built to rewrite Unix. The goal was simple: a language expressive enough to write an OS, portable enough to run on different hardware, and thin enough that you could see the machine through it. C emerged from B (which came from BCPL), each stripping away abstraction. By 1978, Ritchie and Kernighan published *The C Programming Language* — still the clearest language reference ever written.

C became the foundation everything else was built on. Linux, macOS, Windows kernels — all C. Python, Ruby, Node.js interpreters — all C. The JVM, CPython, SQLite — C. Most languages that matter today were written in C or run on a runtime written in C.

## What kind of language is it?
- **Compiled to native machine code** — no interpreter, no VM, no JIT. The output is instructions the CPU executes directly
- **Statically typed** — types are resolved at compile time; the compiler rejects most type errors before the program runs
- **Manually memory-managed** — you call `malloc`/`free`. No garbage collector, no reference counting. Memory lives exactly as long as you say it does
- **No runtime** — there is no invisible layer managing your program. When `main()` is called, you're essentially alone with the OS
- **Portable abstraction over hardware** — C code compiles on x86, ARM, RISC-V, embedded microcontrollers. The same source, different targets

## The C mindset

**You are in control — and responsible for everything.**

In Python or Go, the runtime handles memory, panics are caught, bounds are checked. In C, none of that happens unless you write it. This is the core shift: C trusts the programmer completely. That trust is both the power and the danger.

**Deterministic by default.** C programs do what the code says, in the order it says. There is no garbage collector running between lines, no async scheduler, no hidden allocation. When you call a function, you can reason about exactly what happens to the CPU and memory. This makes C programs predictable and auditable in a way that GC'd languages are not.

**No hidden costs.** Every abstraction in C is visible. A function call is a jump. A struct field access is a memory offset. An array index is pointer arithmetic. There are no surprise allocations, no hidden copies, no lazy evaluation. If you care about performance, this is essential — you can reason about what the machine is doing.

**Undefined behavior as discipline.** C has UB (undefined behavior) — things the standard says can do anything: signed integer overflow, out-of-bounds access, dereferencing null. UB is not a flaw; it's the language telling you there are rules to know and follow. Understanding UB teaches you to think like the machine.

**Testable and controllable.** Because C is deterministic and has no hidden state, C code is highly testable when written well. Pure functions with no side effects are easy to unit test. Memory allocation can be replaced with stubs. The lack of a runtime means you can instrument and control almost any behavior.

**Think in memory.** The mental model shift C demands: stop thinking about values and start thinking about memory. Every variable is a location. Every pointer is an address. Every function call manipulates the stack. This model becomes second nature — and it makes you a better programmer in every other language too.

## Who uses it and for what?
C dominates anywhere performance and control matter more than developer ergonomics:
- **Operating system kernels** — Linux, macOS XNU, Windows NT, BSD, all written in C
- **Embedded and firmware** — microcontrollers, IoT devices, real-time systems where there is no OS at all
- **Compilers and interpreters** — GCC, Clang, CPython, Ruby MRI, Lua
- **Databases** — SQLite, PostgreSQL, MySQL internals
- **Game engines** — low-level rendering layers, physics engines
- **Networking and security** — OpenSSL, libcurl, networking stacks
- **Anywhere C++ isn't needed** — C++ adds complexity; C is often preferred for its simplicity in systems work

## Context today (2026)
C is not going away. It is the language of the machine. Rust is the most serious challenge to C's dominance in systems work — it offers memory safety without a GC — but Rust's adoption in the kernel is still small. Embedded development remains C by default. Most low-level interfaces you'll ever call (POSIX, Win32, libc) are defined in terms of C types and conventions.

Learning C is not about writing new C applications. It is about understanding how software actually works. After C, memory, pointers, the stack, assembly — none of it is mysterious.

## Pros
- Maximum control over memory, execution, and machine resources
- No runtime means no hidden overhead and no pauses
- Portable — runs on any CPU architecture
- Predictable performance — what you write is what the machine does
- Small language — the whole standard fits in your head
- Universal FFI — almost every language can call C code

## Cons
- Manual memory management is hard to get right — leaks, use-after-free, double-free
- No bounds checking by default — buffer overflows are a class of security vulnerabilities
- Undefined behavior traps for the unaware
- No namespaces, no generics, no operator overloading — the language is low-level by design
- Header file / translation unit model adds friction compared to modern module systems
- Debugging is harder — no stack traces in production, no GC metadata

## Recommendations
Learn C when you want to understand how software actually works — memory, the stack, pointers, system calls, compilers. It is the right language for operating systems, embedded development, performance-critical libraries, and anything that needs to run without a runtime.

Do not reach for C for application code, web services, or anything where developer velocity matters more than control. Go covers most backend use cases with far less friction. Rust is the right choice when you want C-level control with memory safety enforced by the compiler.

Write C to learn. Read C to understand libraries and kernels. Use it in production when nothing else will do.

---

## Compiling and running

C source goes through four stages before it becomes a runnable binary:

```
source.c → [preprocessor] → [compiler] → [assembler] → source.o → [linker] → program
```

1. **Preprocessor** — expands `#include`, `#define`, `#ifdef`. Pure text substitution.
2. **Compiler** — translates C to assembly. Where type checking, optimization, and UB analysis happen.
3. **Assembler** — translates assembly to machine code (`.o` object file). One `.o` per `.c` file.
4. **Linker** — combines object files + libraries into the final executable. Resolves cross-file references.

`gcc` and `clang` run all four stages in one command:

```bash
# Recommended: warnings on, C11 standard
gcc -Wall -Wextra -std=c11 -o program main.c

# With debug symbols (use with gdb, valgrind)
gcc -Wall -Wextra -std=c11 -g -o program main.c

# Optimized release build
gcc -Wall -Wextra -std=c11 -O2 -o program main.c

# Multiple source files
gcc -Wall -Wextra -std=c11 -o program main.c utils.c

# Run it
./program

# Check exit code (0 = success, non-zero = failure)
echo $?
```

Key flags:

| Flag | What it does |
|------|-------------|
| `-Wall` | Enable common warnings |
| `-Wextra` | Enable extra warnings beyond `-Wall` |
| `-std=c11` | Use C11 (modern, widely supported) |
| `-g` | Include debug symbols |
| `-O2` | Optimize for speed |
| `-o name` | Set output filename |
| `-c` | Compile only — produce `.o`, skip linking |

Treat warnings as errors while learning: add `-Werror`. A warning in C often means undefined behavior or a real bug.

---

## Conventions

### Naming

C has no namespaces, so naming discipline prevents collisions:

| Thing | Convention | Example |
|-------|-----------|---------|
| Variables and functions | `snake_case` | `read_input`, `byte_count` |
| Constants (`#define`) | `UPPER_SNAKE_CASE` | `MAX_SIZE`, `BUFFER_LEN` |
| Struct names | `snake_case` | `struct node`, `struct tcp_header` |
| Typedef'd types | `snake_case_t` | `uint8_t`, `node_t` |
| File-private functions | `static` | `static void helper(void)` |
| Public library API | prefix with lib name | `mylib_init()`, `mylib_free()` |

### Types: prefer explicit widths

`int` and `long` are platform-defined sizes. Use fixed-width types when size matters:

```c
#include <stdint.h>

uint8_t  byte_val;    // 8-bit unsigned   (0–255)
int32_t  counter;     // 32-bit signed
uint64_t file_size;   // 64-bit unsigned
size_t   len;         // platform-native unsigned — use for sizes and array indices
```

Use `size_t` for lengths, counts, and indices — it matches pointer size and is what the standard library expects.

### Headers and the one-definition rule

Split code into headers (declarations) and `.c` files (definitions):

```c
// math.h — declarations only
#ifndef MATH_H          // header guard: prevents double-inclusion
#define MATH_H

int add(int a, int b);

#endif
```

```c
// math.c — definitions
#include "math.h"

int add(int a, int b) { return a + b; }
```

A function may be declared many times (in headers) but defined exactly once (in one `.c` file). Violating this causes a linker error.

### Error handling: return codes

C has no exceptions. Errors are return values:

```c
FILE *f = fopen("data.txt", "r");
if (f == NULL) {           // always check for NULL/error returns
    perror("fopen");       // prints: "fopen: No such file or directory"
    return 1;
}

void *buf = malloc(1024);
if (buf == NULL) {
    fclose(f);
    return 1;
}

free(buf);                 // always free what you malloc
fclose(f);
```

Check every return value that can fail. `NULL`, `-1`, and non-zero return codes are the C error signaling convention.
