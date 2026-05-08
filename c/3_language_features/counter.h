#ifndef COUNTER_H
#define COUNTER_H
// header guard: preprocessor skips this entire file on second inclusion
// without it, double-including this header causes redefinition errors

#include <stdint.h>

// ── Public types ──────────────────────────────────────────────────────────────

typedef enum {
    COUNTER_OK    = 0,
    COUNTER_OVERFLOW,
    COUNTER_UNDERFLOW,
} CounterStatus;
// typedef gives the enum a name usable without the `enum` keyword
// values are sequential integers starting at 0 unless overridden

typedef struct {
    int32_t value;
    int32_t min;
    int32_t max;
} Counter;
// callers can use Counter directly — implementation details are in counter.c

// ── Public API — declarations only ───────────────────────────────────────────

Counter        counter_create(int32_t min, int32_t max);
CounterStatus  counter_increment(Counter *c);
CounterStatus  counter_decrement(Counter *c);
void           counter_reset(Counter *c);
int32_t        counter_value(const Counter *c);
// const Counter *c — caller promises not to modify *c through this pointer
// signals read-only intent at the API boundary

#endif // COUNTER_H
