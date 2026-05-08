#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// ── Pointers ─────────────────────────────────────────────────────────────────

void demo_pointers() {

    int x = 42;
    // stack allocated, lives at some address in memory

    int *p = &x;
    // p holds the ADDRESS of x (e.g., 0x7ffd...)
    // *p dereferences — follows the address to read/write the value

    *p = 100;
    // writes 100 into the memory location x occupies
    // x is now 100

    int **pp = &p;
    // pointer to a pointer — holds the address of p itself
    // **pp == x, *pp == p

    int *null_p = NULL;
    // NULL is 0 cast to a pointer — represents "no valid address"
    // dereferencing NULL is undefined behavior (usually a segfault)

    (void)pp; (void)null_p;
}

// ── Pointer arithmetic ────────────────────────────────────────────────────────

void demo_pointer_arithmetic() {

    int arr[4] = {10, 20, 30, 40};

    int *p = arr;
    // p points to arr[0]

    p++;
    // p now points to arr[1]
    // under the hood: address += sizeof(int) (not += 1)
    // arithmetic is always in units of the pointed-to type

    int val = *(p + 2);
    // p + 2 is arr[3] — address + 2 * sizeof(int)
    // val == 40

    ptrdiff_t diff = (arr + 3) - arr;
    // pointer subtraction gives element count, not byte count
    // diff == 3

    // valid range: p may point to arr[0]..arr[4]
    // arr[4] is ONE past the end — legal to point to, illegal to dereference

    (void)val; (void)diff;
}

// ── Arrays vs pointers ────────────────────────────────────────────────────────

void demo_arrays_vs_pointers() {

    int arr[4] = {1, 2, 3, 4};
    // arr is NOT a pointer — it is the array itself, stack allocated
    // sizeof(arr) == 4 * sizeof(int) == 16

    int *p = arr;
    // arr DECAYS to a pointer to its first element in most expressions
    // sizeof(p) == 8 (pointer size on 64-bit)
    // decay loses the length — p has no idea it points to 4 elements

    // arr[i] and *(arr + i) are identical — the compiler generates the same code
    int a = arr[2];
    int b = *(p + 2);
    // a == b == 3

    // what you CANNOT do with an array that you can with a pointer:
    // arr++;        — illegal: arr is not an lvalue, its address is fixed
    // arr = p;      — illegal: cannot reassign an array

    // function parameter decay:
    // void f(int arr[4]) is IDENTICAL to void f(int *arr)
    // the size annotation is ignored — the array decays at the call site

    (void)a; (void)b;
}

// ── Stack vs heap ─────────────────────────────────────────────────────────────

void demo_stack() {

    int x = 10;
    // stack: allocated automatically when function is entered
    // freed automatically when function returns
    // fast: just a stack pointer decrement
    // limited: typically ~1–8 MB total per thread

    int arr[1000];
    // also stack — 1000 * 4 = 4000 bytes gone from the stack frame
    // no initialization — contains whatever was there before

    (void)x; (void)arr;
}

void demo_heap() {

    int *p = malloc(4 * sizeof(int));
    // heap: manually allocated from the OS via libc
    // survives until you call free() — not tied to function scope
    // slower than stack: requires bookkeeping and possible syscall
    // size only limited by available memory

    if (p == NULL) {
        // malloc returns NULL if allocation fails — always check
        return;
    }

    p[0] = 1;
    p[1] = 2;
    p[2] = 3;
    p[3] = 4;

    free(p);
    // returns memory to the allocator
    // p still holds the old address — now dangling
    // do not read or write through p after free

    p = NULL;
    // defensive: NULL-ing prevents accidental use-after-free
}

void demo_heap_resize() {

    int *p = malloc(4 * sizeof(int));
    if (p == NULL) return;

    int *bigger = realloc(p, 8 * sizeof(int));
    // realloc grows or shrinks the allocation
    // may return a NEW address if it had to move the block
    // on failure returns NULL — original pointer is still valid

    if (bigger == NULL) {
        free(p);
        return;
    }

    p = bigger;
    // p now points to 8 ints
    // old p is invalid — realloc already handled it

    free(p);
}

// ── Undefined behavior ────────────────────────────────────────────────────────

void demo_ub_examples() {

    // 1. Out-of-bounds access
    int arr[4] = {0};
    // arr[4] = 1;
    // writes past the array — the memory belongs to something else
    // may corrupt adjacent variables, stack frame, or segfault
    // the compiler may optimize assuming this never happens

    // 2. Use after free
    int *p = malloc(sizeof(int));
    free(p);
    // *p = 5;
    // the allocator may have reused that memory
    // reading/writing it is UB — not just "probably wrong"

    // 3. Signed integer overflow
    int max = 2147483647;
    // int result = max + 1;
    // signed overflow is UB — NOT defined as wrap-around
    // the compiler assumes overflow never happens and may remove checks
    // use uint32_t or unsigned int if you need wrap-around

    // 4. Uninitialized read
    int x;
    // printf("%d\n", x);
    // x contains whatever bits were in that stack location
    // compilers may assume it has any value and optimize accordingly

    // 5. Null pointer dereference
    int *null_p = NULL;
    // *null_p = 1;
    // always a segfault in practice, but technically UB
    // the compiler is allowed to assume this never happens
    // and may optimize away checks that depend on it

    (void)arr; (void)p; (void)max; (void)x; (void)null_p;
    // suppress unused-variable warnings on the demo vars
}

// ── Dynamic memory patterns ───────────────────────────────────────────────────

struct Buffer {
    uint8_t *data;
    size_t   len;
    size_t   cap;
};

struct Buffer buffer_create(size_t initial_cap) {

    struct Buffer b;
    b.data = malloc(initial_cap);
    b.len  = 0;
    b.cap  = (b.data != NULL) ? initial_cap : 0;
    // ownership is established here — caller must call buffer_free()
    return b;
}

void buffer_free(struct Buffer *b) {

    free(b->data);
    b->data = NULL;
    b->len  = 0;
    b->cap  = 0;
    // zeroing fields after free prevents double-free bugs
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(void) {

    demo_pointers();
    demo_pointer_arithmetic();
    demo_arrays_vs_pointers();
    demo_stack();
    demo_heap();
    demo_heap_resize();
    demo_ub_examples();

    struct Buffer buf = buffer_create(64);
    buffer_free(&buf);

    return 0;
}
