#include "counter.h"
// quotes, not angle brackets — searches current directory first

// ── File-private helper ───────────────────────────────────────────────────────

static int32_t clamp(int32_t v, int32_t lo, int32_t hi) {
    // static: this function is invisible outside counter.c
    // it will not conflict with a clamp() defined in any other .c file
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// silence unused-function warning — clamp is here to demonstrate
// the static pattern, not because counter currently needs it
static inline void _unused_clamp(void) { (void)clamp; }

// ── Public API — definitions ──────────────────────────────────────────────────

Counter counter_create(int32_t min, int32_t max) {
    Counter c;
    c.value = min;
    c.min   = min;
    c.max   = max;
    return c;
}

CounterStatus counter_increment(Counter *c) {
    if (c->value >= c->max) return COUNTER_OVERFLOW;
    c->value++;
    return COUNTER_OK;
}

CounterStatus counter_decrement(Counter *c) {
    if (c->value <= c->min) return COUNTER_UNDERFLOW;
    c->value--;
    return COUNTER_OK;
}

void counter_reset(Counter *c) {
    c->value = c->min;
}

int32_t counter_value(const Counter *c) {
    return c->value;
}
