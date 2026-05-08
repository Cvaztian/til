#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_STR_SIZE 64
#define RB_CAPACITY  10

// ── Beginner ──────────────────────────────────────────────────────────────────

// Problem 1: Safe String Length
// Implement: size_t my_strlen(const char *s);
// Walk the string byte by byte until you hit the null terminator '\0'.
// Guard against NULL input — real strlen crashes on it.
// Focus: pointer iteration, null terminator, size_t vs int.

size_t my_strlen(const char *s) {
    if (s == NULL) return 0;
    size_t result = 0;
    while (*s++) result++;
    return result;
}

// Problem 2: Reverse String In Place
// Implement: void reverse(char *s);
// Swap characters from both ends walking toward the middle.
// Stop when the two pointers meet or cross.
// Focus: pointer arithmetic, in-place mutation, index vs pointer style.

void reverse(char *s) {
    if (s == NULL) return;

    char *p = s;
    size_t len = 0;
    while (*p++) len++;

    size_t last_idx = len - 1;
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = *(s + i);
        *(s + i) = *(s + last_idx - i);
        *(s + last_idx - i) = tmp;
    }
}

// Problem 3: Implement memcpy
// Implement: void *my_memcpy(void *dst, const void *src, size_t n);
// Copy n bytes from src to dst one byte at a time.
// Cast both pointers to unsigned char* to do byte-level access.
// Do not handle overlapping regions (that's memmove — a separate problem).
// Focus: void* casting, byte-level memory, why unsigned char and not char.

void *my_memcpy(void *dst, const void *src, size_t n) {
    if (dst == NULL || src == NULL) return dst;
    if (n == 0) return dst;
    for (size_t i = 0; i < n; i++)
        ((unsigned char *)dst)[i] = ((const unsigned char *)src)[i];
    return dst;
}

// ── Intermediate ──────────────────────────────────────────────────────────────

// Problem 4: Ring Buffer
// Implement a fixed-size circular queue over a static array:
//   void    rb_push(RingBuffer *rb, uint8_t val);
//   uint8_t rb_pop(RingBuffer *rb);
//   int     rb_empty(const RingBuffer *rb);
// Use head/tail indices and advance them with modulo (% capacity).
// Decide how to distinguish full from empty — one common approach: keep a count.
// Focus: modular arithmetic, index wrap-around, embedded queue patterns.

typedef struct {
    uint8_t data[RB_CAPACITY];
    int head;
    int tail;
    int counter;
} RingBuffer;

void rb_push(RingBuffer *rb, uint8_t val) {
    if (rb->counter == RB_CAPACITY) return;
    rb->data[rb->tail] = val;
    rb->tail = (rb->tail + 1) % RB_CAPACITY;
    rb->counter++;
}

uint8_t rb_pop(RingBuffer *rb) {
    if (rb->counter == 0) return 0;
    uint8_t result = rb->data[rb->head];
    rb->head = (rb->head + 1) % RB_CAPACITY;
    rb->counter--;
    return result;
}

int rb_empty(const RingBuffer *rb) {
    return rb->counter == 0;
}

// Problem 5: Bitfield Parser
// Given a 32-bit packet with this layout:
//   bits 31-24: source address  (8 bits)
//   bits 23-16: destination     (8 bits)
//   bits 15-8:  payload length  (8 bits)
//   bits 7-0:   flags           (8 bits)
// Write functions to extract each field using masks and shifts.
// Then set individual flag bits (bit 0 = ACK, bit 1 = SYN, bit 2 = FIN).
// Focus: masks, shifts, why endianness matters when reading from bytes vs uint32_t.

// Problem 6: Safe Integer Parser
// Implement: int parse_int(const char *s, int *out);
// Parse a decimal integer string into *out. Return 0 on success, -1 on error.
// Reject: NULL, empty string, non-numeric characters, overflow (> INT_MAX / < INT_MIN).
// Use strtol — check errno and the end pointer to detect all failure modes.
// Focus: strtol + errno pattern, input validation at a system boundary, error codes.

// ── Advanced ──────────────────────────────────────────────────────────────────

// Problem 7: State Machine
// Model a simple process lifecycle:
//   INIT → READY → RUNNING → INIT  (reset)
//                → ERROR   (any state can fault)
// Define states as an enum. Write a transition function:
//   State transition(State current, Event event);
// Use a switch on (state, event) pairs. Return the current state unchanged
// for invalid transitions rather than crashing.
// Focus: deterministic design, exhaustive switch, illegal transition handling.

// Problem 8: Singly Linked List
// Implement a linked list of ints:
//   Node *list_push(Node *head, int val);  // prepend, return new head
//   Node *list_pop(Node *head, int *out);  // remove head, return new head
//   void  list_free(Node *head);           // free every node
// Each node is heap-allocated with malloc. list_free must walk and free each node.
// Focus: malloc per node, pointer-to-next ownership, freeing in traversal order.

// Problem 9: Command Parser
// Parse lines of the form "VERB ARG1 ARG2" (e.g. "SET SPEED 120", "GET TEMP").
// Use strtok to tokenize, then dispatch to handlers via a lookup table of
// { name, handler } structs rather than a chain of strcmp if/else.
// Reject unknown verbs and missing arguments explicitly.
// Focus: strtok, function pointer dispatch, defensive parsing.

// Problem 10: Simulated Device Driver
// Model a fake UART peripheral using a struct with volatile fields:
//   typedef struct { volatile uint8_t status; volatile uint8_t data; } UART;
// Implement:
//   void    uart_send(UART *uart, uint8_t byte);  // wait on status, write data
//   uint8_t uart_recv(UART *uart);                // wait on status, read data
// Simulate the hardware side by toggling status flags from a helper.
// Focus: volatile, memory-mapped I/O abstraction, polling loops.

// ── main ──────────────────────────────────────────────────────────────────────

int main(void) {

    // Problem 1
    const char *ro_samples[] = {"abc", "-1", ""};
    for (int i = 0; i < 3; i++)
        printf("strlen(\"%s\") = %zu\n", ro_samples[i], my_strlen(ro_samples[i]));

    // Problem 2
    char mut_samples[][MAX_STR_SIZE] = {"def", "+1", ""};
    for (int i = 0; i < 3; i++) {
        printf("reverse(\"%s\")", mut_samples[i]);
        reverse(mut_samples[i]);
        printf(" = \"%s\"\n", mut_samples[i]);
    }

    // Problem 3
    int src[] = {1, 2, 3};
    int dst[3];
    size_t n = sizeof(src) / sizeof(src[0]);
    my_memcpy(dst, src, n * sizeof(int));
    printf("memcpy: ");
    for (size_t i = 0; i < n; i++)
        printf("%d ", dst[i]);
    printf("\n");

    // Problem 4
    RingBuffer rb = {.head = 0, .tail = 0, .counter = 0};

    rb_push(&rb, 1);
    rb_push(&rb, 2);
    rb_push(&rb, 3);
    printf("rb after push 1,2,3 — empty: %d\n", rb_empty(&rb));
    printf("rb_pop: %d\n", rb_pop(&rb));
    printf("rb_pop: %d\n", rb_pop(&rb));
    printf("rb_pop: %d\n", rb_pop(&rb));
    printf("rb after drain — empty: %d\n", rb_empty(&rb));
    printf("rb_pop empty: %d\n", rb_pop(&rb));

    for (int i = 0; i < RB_CAPACITY + 2; i++)
        rb_push(&rb, (uint8_t)i);
    printf("rb after overfill — counter: %d (expected %d)\n", rb.counter, RB_CAPACITY);

    return 0;
}
