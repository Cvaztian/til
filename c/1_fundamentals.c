#include <stdint.h>
#include <string.h>
#include <stdio.h>
// Preprocessor directive to copy in the declarations from the standard
// input/output library from stdio.h header file, located at libc.so
// Includes functions: printf, scanf, fprintf, fopen, etc.

int global_var = 10;
// lives in .data segment
// exists for entire program lifetime
// initialized before main() via loader/runtime

void demo_variables() {

    int local_int = 42;
    // stack allocated (automatic storage)
    // lifetime = until function returns

    uint8_t small = 255;
    // fixed-width 8-bit unsigned integer
    // wrap-around behavior is defined (mod 256)

    int signed_int = 2147483647;
    // signed overflow is undefined behavior
    // compiler assumes it never happens

    float approx = 3.14f;
    // IEEE-754 approximation (not exact decimal)

    char letter = 'A';
    // ASCII encoded integer stored in 1 byte
}

void demo_loops() {

    int sum = 0;

    for (int i = 0; i < 5; i++) {
        // compiled into:
        // init, compare, body, increment, jump
        sum += i;
    }

    int j = 0;

    while (j < 3) {
        // conditional branch loop
        j++;
    }
}

int add(int a, int b) {
    // arguments are COPIED into registers/stack (ABI dependent)

    int sum = a + b;

    // return value passed via CPU register (e.g., eax/rax)
    return sum;
}

void demo_functions() {

    int result = add(10, 20);
    // call creates:
    // - new stack frame
    // - register save/restore
    // - jump to function address

    printf("Result: %d\n", result);
}

void demo_arrays() {

    int arr[4] = {1, 2, 3, 4};
    // contiguous block of 4 * sizeof(int)
    // no metadata (no length stored)

    int *p = arr;
    // array decays to pointer to first element in expressions

    int sum = 0;

    for (int i = 0; i < 4; i++) {
        // arr[i] == *(arr + i)
        sum += p[i];
    }
}

void demo_strings() {

    char str[] = "Hello";
    // memory: ['H','e','l','l','o','\0']
    // '\0' is required sentinel for string termination

    printf("%s\n", str);
    // reads memory until null terminator is found

    // similar, but dangerous:
    // char *bad = "Hello";
    // stored in read-only segment (.rodata)
}

struct Person {
    char name[20];
    // inline fixed-size memory block

    int age;
    // aligned (often 4 bytes)
    // compiler may insert padding between fields
};

void demo_structs() {

    struct Person p;

    strcpy(p.name, "Sebastian");
    p.age = 24;

    // memory layout:
    // [ name .............. ][ age ][padding? ]
    // struct is contiguous memory with offsets
}

int main() {

    // The OS does NOT start here directly.
    // Execution flow:
    // kernel → loader → _start → libc init → main()

    demo_variables();
    demo_loops();
    demo_functions();
    demo_arrays();
    demo_strings();
    demo_structs();

    // return value is part of OS contract:
    // 0   = success
    // !=0 = failure

    // used by:
    // - shell scripts
    // - build systems (make, CI pipelines)
    // - parent processes

    return 0;
}