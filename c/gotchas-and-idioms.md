# C Gotchas and Idioms
> **Tags:** `c`, `gotchas`, `idioms`, `pointers`, `strings`, `undefined-behavior`
> **Date:** 2026-05-08
> **Related:** [0_about.md](0_about.md), [2_pointers_and_memory.c](2_pointers_and_memory.c), [4_code_quality.c](4_code_quality.c)

Practical discoveries from working through C exercises — traps, idioms, and non-obvious behavior.

---

## String literals live in read-only memory

```c
char *s = "hello";   // pointer into .rodata — writes will segfault
char s[] = "hello";  // copy on the stack — mutable
```

`char *` pointing to a string literal looks like a mutable string but isn't. The OS maps `.rodata` as read-only and kills the process on any write. Always use `char[]` when you need to mutate the string.

For arrays of strings:
```c
const char *samples[] = {"abc", "def"};      // read-only — default choice
char samples[][64]    = {"abc", "def"};       // mutable — use when needed
```

---

## Operator precedence trap

```c
while (s += result != '\0')  // parsed as: s += (result != '\0')
while (*(s++) != '\0')       // what you meant
```

`!=` binds tighter than `+=`. When in doubt, parenthesize — or use a separate increment.

---

## `size_t` and its format specifier

`size_t` is the correct type for lengths, counts, and array indices. It's unsigned and pointer-sized (64-bit on 64-bit systems). Its `printf` format specifier is `%zu` — using `%d` is undefined behavior on platforms where `size_t` is wider than `int`.

```c
printf("%zu\n", my_strlen(s));  // correct
printf("%d\n",  my_strlen(s));  // UB on 64-bit
```

---

## Array length idiom

C arrays carry no length metadata. The idiomatic way to get the count at the declaration site:

```c
int arr[] = {1, 2, 3, 4};
size_t n = sizeof(arr) / sizeof(arr[0]);
```

This breaks as soon as the array decays to a pointer (e.g. when passed to a function) — `sizeof(ptr)` gives the pointer size, not the array size. Always compute length before the decay.

---

## Pointer subtraction gives element count, not bytes

```c
int arr[4] = {1, 2, 3, 4};
ptrdiff_t diff = (arr + 3) - arr;  // == 3, not 12
```

Pointer arithmetic always works in units of the pointed-to type. Subtraction divides the byte distance by `sizeof(type)` automatically. The result type is `ptrdiff_t` — a signed integer, because the difference can be negative.

---

## Reading complex pointer declarations

Read from the variable name outward, right before left:

```c
const int *p;         // p is a pointer to const int — value is read-only
int * const p;        // p is a const pointer to int — pointer is read-only
const int * const p;  // both are read-only
```

For casts: the cast applies to the pointer, the dereference applies to the value:

```c
((unsigned char *)src)[i]   // cast src to unsigned char*, then index into it
(char*)*(src + i)           // wrong: casts the dereferenced VALUE, not the pointer
```

---

## `volatile` — preventing compiler register caching

The compiler assumes a variable only changes when your code writes to it. If nothing in a loop writes to `flag`, it may read it once, cache it in a register, and loop forever.

```c
volatile int flag = 0;  // re-read from memory on every access
```

`volatile` tells the compiler: "something outside your view can change this." Use it for hardware registers, signal handlers, and interrupt-shared variables in embedded code.

**What `volatile` does not do:** it provides no atomicity and no thread safety. For multi-threaded code, use `_Atomic` or a mutex.

---

## Null pointer dereference — UB, not just a crash

Dereferencing `NULL` is undefined behavior. In practice it always segfaults, but the compiler is allowed to assume it never happens — and may optimize away checks that depend on it:

```c
void f(int *p) {
    *p = 1;            // compiler concludes: p cannot be NULL here
    if (p == NULL)     // ...so this branch is dead code and may be eliminated
        handle_error();
}
```

The null check disappears before the segfault can happen. Guard at the top, before any dereference.

---

## `strnlen` over `strlen` for untrusted input

`strlen` reads until it finds `'\0'` with no bound — if the input has no null terminator it walks off into undefined memory. `strnlen` stops at a limit:

```c
size_t len = strnlen(input, MAX_LEN + 1);
// if len > MAX_LEN, the string is too long (or untrusted)
```

Requires `#define _POSIX_C_SOURCE 200809L` before includes when compiling with `-std=c11`.

---

## `printf` format string security

Passing user-controlled data as the format string is a classic vulnerability:

```c
printf(user_input);         // dangerous — attacker passes "%n" to write memory
printf("%s", user_input);   // safe — format string is a hardcoded literal
```

`%n` writes the number of bytes printed so far to a pointer argument, giving an attacker arbitrary memory writes. The rule: **the format string must always be a string literal**.

---

## `void *` — the universal pointer

`void *` accepts any pointer type without an explicit cast. It's how C implements generic interfaces:

```c
void *my_memcpy(void *dst, const void *src, size_t n);

int src[] = {1, 2, 3};
int dst[3];
my_memcpy(dst, src, 3 * sizeof(int));  // no cast needed
```

`n` is always in **bytes** — the caller is responsible for multiplying by `sizeof(type)` for non-char types. Inside the function, cast to `unsigned char *` to do byte-level access (`char` can be signed; `unsigned char` is always 1 byte with defined behavior).

---

## Ring buffer — drop vs overwrite when full

A ring buffer tracks `head` (read) and `tail` (write) indices that wrap with modulo. Two strategies when full:

**Drop new data** — reject the write, keep existing data intact. Default for input buffers (UART, keyboard) where losing new data is less harmful than corrupting old.

**Overwrite oldest** — advance head along with tail, discarding the oldest entry. Used for logging and telemetry where you want the most recent N items.

The static array is intentional in embedded contexts: no heap, no fragmentation risk, allocation guaranteed at compile time.

---

## `return` in void functions

`return;` with no value is valid in a `void` function but redundant as the last statement — the function returns automatically at the closing brace. Reserve it for early exits:

```c
void process(char *s) {
    if (s == NULL) return;   // early exit — useful
    // ...
}                            // implicit return — no need to write return;
```
