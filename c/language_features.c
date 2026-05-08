#include <stdio.h>
#include <stdint.h>
#include "counter.h"
// local header — uses the Counter type and CounterStatus enum defined there

// ── Bitwise operations ────────────────────────────────────────────────────────

void demo_bitwise() {

    uint8_t a = 0b00111100;  // 60
    uint8_t b = 0b00001111;  // 15

    uint8_t and = a & b;
    // AND: bit is 1 only if BOTH inputs are 1
    // 0b00001100 == 12

    uint8_t or = a | b;
    // OR: bit is 1 if EITHER input is 1
    // 0b00111111 == 63

    uint8_t xor = a ^ b;
    // XOR: bit is 1 if inputs DIFFER
    // 0b00110011 == 51

    uint8_t not = ~a;
    // NOT: flips every bit
    // 0b11000011 == 195

    uint8_t left  = a << 2;
    // left shift: moves bits left, fills right with 0
    // equivalent to multiplying by 2^n (when no bits overflow)
    // 0b11110000 == 240

    uint8_t right = a >> 2;
    // right shift: moves bits right
    // for unsigned types: fills left with 0 (logical shift)
    // for signed types: implementation-defined (usually arithmetic — fills with sign bit)
    // 0b00001111 == 15

    // ── common patterns ──

    uint8_t flags = 0;

    flags |= (1 << 3);
    // SET bit 3: OR with a mask that has only bit 3 set

    flags &= ~(1 << 3);
    // CLEAR bit 3: AND with the complement of the mask

    flags ^= (1 << 3);
    // TOGGLE bit 3: XOR flips the bit regardless of current state

    int is_set = (flags >> 3) & 1;
    // TEST bit 3: shift it to position 0, mask off all other bits

    (void)and; (void)or; (void)xor; (void)not;
    (void)left; (void)right; (void)is_set;
}

// ── const ─────────────────────────────────────────────────────────────────────

void demo_const() {

    const int x = 42;
    // x cannot be modified — compiler error if you try
    // not a compile-time constant like #define; it has a memory address

    int value = 10;

    const int *pc = &value;
    // pointer to const int: the VALUE cannot be changed through pc
    // but pc itself can be reassigned to point elsewhere
    // *pc = 5;    — compile error
    // pc = &x;    — allowed

    int * const cp = &value;
    // const pointer to int: the POINTER cannot be reassigned
    // but the value it points to can be modified
    // *cp = 5;    — allowed
    // cp = &x;    — compile error

    const int * const cpc = &value;
    // const pointer to const int: neither the pointer nor the value can change
    // read from left to right: "const int, accessed via const pointer"

    // const in function parameters signals intent:
    // void print(const char *s)  — I will not modify the string
    // void sort(int *arr, size_t n)  — I may modify the array

    (void)x; (void)pc; (void)cp; (void)cpc;
}

// ── volatile ──────────────────────────────────────────────────────────────────

void demo_volatile() {

    // the compiler optimizes code by assuming variables only change
    // when the program itself writes to them.
    // volatile tells it: "this value can change outside your control"

    volatile int flag = 0;
    // without volatile, the compiler might cache flag in a register
    // and never re-read it from memory in a loop.
    // with volatile, every access goes to memory.

    // typical use cases:
    // 1. hardware registers — memory-mapped I/O, value changes by hardware
    //    volatile uint32_t *uart_status = (volatile uint32_t *)0x40001000;
    //
    // 2. signal handlers — a signal may set a flag your main loop checks
    //    volatile sig_atomic_t interrupted = 0;
    //
    // 3. shared memory with interrupts (embedded/bare-metal)

    // volatile does NOT provide atomicity or thread safety.
    // for multi-threaded code, use _Atomic or platform mutexes.

    (void)flag;
}

// ── enums ─────────────────────────────────────────────────────────────────────

typedef enum {
    STATE_IDLE    = 0,
    STATE_RUNNING = 1,
    STATE_PAUSED  = 2,
    STATE_STOPPED = 3,
} State;
// typedef: usable as `State` instead of `enum State`
// values are explicit here — document them when the integers matter

typedef enum {
    PERM_NONE    = 0,
    PERM_READ    = 1 << 0,  // 0b001
    PERM_WRITE   = 1 << 1,  // 0b010
    PERM_EXECUTE = 1 << 2,  // 0b100
} Permission;
// power-of-two values allow combining with bitwise OR:
// Permission p = PERM_READ | PERM_WRITE;

void demo_enums() {

    State s = STATE_IDLE;

    switch (s) {
        case STATE_IDLE:    break;
        case STATE_RUNNING: break;
        case STATE_PAUSED:  break;
        case STATE_STOPPED: break;
        // with -Wextra, the compiler warns if a case is missing
        // that turns enums into exhaustiveness-checked state machines
    }

    Permission perms = PERM_READ | PERM_EXECUTE;

    int can_write = (perms & PERM_WRITE) != 0;
    // test whether a specific permission bit is set

    (void)can_write;
}

// ── Function pointers ─────────────────────────────────────────────────────────

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

typedef int (*BinaryOp)(int, int);
// typedef cleans up the syntax:
// without it: int (*fn)(int, int) = add;
// with it:    BinaryOp fn = add;

void demo_function_pointers() {

    BinaryOp op = add;
    // op holds the address of the add function
    // functions decay to pointers (like arrays decay to element pointers)

    int result = op(3, 4);
    // calls add(3, 4) — indirection through the pointer
    // result == 7

    // ── dispatch table ──

    BinaryOp ops[3] = { add, sub, mul };
    // array of function pointers — indexed dispatch instead of switch

    for (int i = 0; i < 3; i++) {
        printf("%d\n", ops[i](10, 3));
        // prints: 13, 7, 30
    }

    // ── callback pattern ──
    // function pointers are how C passes behavior as a parameter
    // e.g., qsort takes a comparator: int (*compar)(const void *, const void *)

    (void)result;
}

// ── Header / source separation ────────────────────────────────────────────────

void demo_header_source() {

    // counter.h declares the public API — types and function signatures
    // counter.c defines the implementation — the actual function bodies
    // this file includes counter.h and uses Counter without knowing the internals

    Counter c = counter_create(0, 5);
    // Counter is defined in counter.h as a typedef'd struct
    // counter_create is declared in counter.h, defined in counter.c

    CounterStatus status = counter_increment(&c);
    // CounterStatus is an enum defined in counter.h
    // values: COUNTER_OK, COUNTER_OVERFLOW, COUNTER_UNDERFLOW

    if (status == COUNTER_OK) {
        printf("value: %d\n", counter_value(&c));
    }

    // compile with both .c files:
    // gcc -Wall -Wextra -std=c11 -o program language_features.c counter.c
    //
    // rules:
    // - declarations go in .h (what exists)
    // - definitions go in .c (what it does)
    // - static functions in .c are file-private — no naming conflicts
    // - header guards prevent double-inclusion errors
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(void) {

    demo_bitwise();
    demo_const();
    demo_volatile();
    demo_enums();
    demo_function_pointers();
    demo_header_source();

    return 0;
}
