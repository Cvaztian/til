#define _POSIX_C_SOURCE 200809L
// expose POSIX extensions (strnlen, strerror, etc.) when compiling with -std=c11
// must appear before any #include

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

// ── Logging ───────────────────────────────────────────────────────────────────

// log levels — higher value means more severe
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} LogLevel;

static LogLevel g_log_level = LOG_DEBUG;
// global minimum level — messages below this are suppressed

static const char *level_label[] = { "DEBUG", "INFO", "WARN", "ERROR" };

#define LOG(level, fmt, ...) \
    do { \
        if ((level) >= g_log_level) { \
            fprintf(stderr, "[%s] %s:%d: " fmt "\n", \
                    level_label[level], __FILE__, __LINE__, ##__VA_ARGS__); \
        } \
    } while (0)
// __FILE__, __LINE__ are compiler-provided macros — injected at the call site
// ##__VA_ARGS__ handles the case where no extra args are passed (GNU extension)
// do { } while (0) makes the macro safe inside if/else without braces

#define LOG_DEBUG(fmt, ...) LOG(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG(LOG_INFO,  fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG(LOG_WARN,  fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG(LOG_ERROR, fmt, ##__VA_ARGS__)
// shorthand macros — call site reads: LOG_WARN("retrying, attempt %d", n)

// log to stderr, not stdout:
// stdout is for program output (may be piped or redirected)
// stderr is for diagnostics — always visible even when stdout is piped

// ── Assertions ────────────────────────────────────────────────────────────────

// assert() from <assert.h>
// - evaluates the expression; if false, prints a message and calls abort()
// - disabled entirely when compiled with -DNDEBUG (release builds)
// - use for invariants — conditions that MUST be true by contract
// - do NOT use for recoverable runtime errors (user input, malloc failure)

_Static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
// static_assert: evaluated at compile time — zero runtime cost
// use for platform assumptions, struct sizes, alignment requirements
// triggers a compile error (not a crash) if the condition is false

#define ASSERT_NOT_NULL(ptr) \
    assert((ptr) != NULL)
// wrapping assert in a named macro makes intent clear at the call site

// custom assert that logs before aborting — useful in larger codebases
#define REQUIRE(cond) \
    do { \
        if (!(cond)) { \
            LOG_ERROR("assertion failed: " #cond); \
            abort(); \
        } \
    } while (0)
// #cond stringifies the condition — prints the exact expression that failed

// ── Error handling ────────────────────────────────────────────────────────────

// convention: functions return 0 on success, negative on failure
// caller is responsible for checking and propagating

typedef enum {
    ERR_OK          =  0,
    ERR_NULL        = -1,
    ERR_RANGE       = -2,
    ERR_OVERFLOW    = -3,
    ERR_IO          = -4,
    ERR_OOM         = -5,  // out of memory
} ErrorCode;

static const char *error_string(ErrorCode e) {
    switch (e) {
        case ERR_OK:       return "ok";
        case ERR_NULL:     return "null pointer";
        case ERR_RANGE:    return "out of range";
        case ERR_OVERFLOW: return "overflow";
        case ERR_IO:       return "I/O error";
        case ERR_OOM:      return "out of memory";
        default:           return "unknown error";
    }
}

// ── goto cleanup pattern ──────────────────────────────────────────────────────

// the only sanctioned use of goto in C: forward jump to a single cleanup label
// avoids deeply nested if/else when multiple resources need releasing on error

ErrorCode process_file(const char *path, char *out_buf, size_t buf_size) {

    assert(out_buf != NULL);   // caller contract — programmer error if violated
    assert(buf_size > 0);

    ErrorCode result = ERR_OK;

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        LOG_ERROR("fopen(%s): %s", path, strerror(errno));
        // errno is set by the OS; strerror() gives the human-readable message
        result = ERR_IO;
        goto done;
        // skips all cleanup below — nothing to free yet, but the pattern holds
    }

    char *tmp = malloc(buf_size);
    if (tmp == NULL) {
        LOG_ERROR("malloc(%zu) failed", buf_size);
        result = ERR_OOM;
        goto close_file;
        // jump past tmp cleanup — tmp was never allocated
    }

    size_t n = fread(tmp, 1, buf_size - 1, f);
    if (ferror(f)) {
        LOG_WARN("fread incomplete: read %zu bytes", n);
        result = ERR_IO;
        goto free_tmp;
    }

    tmp[n] = '\0';
    memcpy(out_buf, tmp, n + 1);

free_tmp:
    free(tmp);
close_file:
    fclose(f);
done:
    return result;
    // labels are ordered in reverse acquisition order:
    // acquire: f → tmp
    // release: tmp → f
}

// ── Input validation ──────────────────────────────────────────────────────────

// validate at the boundary where untrusted data enters the program.
// inside the codebase, trust your own code and use assert for invariants.

ErrorCode safe_divide(int32_t num, int32_t den, int32_t *out) {

    if (out == NULL) {
        LOG_ERROR("safe_divide: out pointer is NULL");
        return ERR_NULL;
    }
    if (den == 0) {
        LOG_WARN("safe_divide: division by zero");
        return ERR_RANGE;
    }
    // INT32_MIN / -1 overflows — the one signed division edge case
    if (num == INT32_MIN && den == -1) {
        LOG_WARN("safe_divide: signed overflow (INT32_MIN / -1)");
        return ERR_OVERFLOW;
    }

    *out = num / den;
    return ERR_OK;
}

ErrorCode safe_add(int32_t a, int32_t b, int32_t *out) {

    if (out == NULL) return ERR_NULL;

    // check overflow BEFORE doing the arithmetic
    // doing (a + b) first and then checking is already UB if it overflows
    if (b > 0 && a > INT32_MAX - b) {
        LOG_WARN("safe_add: overflow (%d + %d)", a, b);
        return ERR_OVERFLOW;
    }
    if (b < 0 && a < INT32_MIN - b) {
        LOG_WARN("safe_add: underflow (%d + %d)", a, b);
        return ERR_OVERFLOW;
    }

    *out = a + b;
    return ERR_OK;
}

#define MAX_NAME_LEN 64

ErrorCode validate_name(const char *name, size_t *out_len) {

    if (name == NULL) {
        LOG_ERROR("validate_name: null input");
        return ERR_NULL;
    }

    size_t len = strnlen(name, MAX_NAME_LEN + 1);
    // strnlen stops at the limit — safe even if name has no null terminator
    // strlen reads until '\0' with no bound — never use on untrusted input

    if (len == 0) {
        LOG_WARN("validate_name: empty string");
        return ERR_RANGE;
    }
    if (len > MAX_NAME_LEN) {
        LOG_WARN("validate_name: too long (%zu > %d)", len, MAX_NAME_LEN);
        return ERR_RANGE;
    }

    if (out_len != NULL) *out_len = len;
    return ERR_OK;
}

// ── Defensive programming ─────────────────────────────────────────────────────

// 1. initialize everything — never read uninitialized memory
// 2. validate all inputs at the public API boundary
// 3. check every return value that can fail
// 4. fail loudly and early — silent failures corrupt state silently
// 5. release resources on every path, not just the happy path

typedef struct {
    int32_t *data;
    size_t   len;
    size_t   cap;
} IntVec;

ErrorCode intvec_push(IntVec *v, int32_t value) {

    if (v == NULL) return ERR_NULL;
    // guard even internal functions when the pointer comes from the caller

    if (v->len == v->cap) {

        size_t new_cap;
        if (v->cap == 0) {
            new_cap = 8;
        } else {
            // check for multiplication overflow before doing it
            if (v->cap > SIZE_MAX / 2) {
                LOG_ERROR("intvec_push: capacity overflow");
                return ERR_OVERFLOW;
            }
            new_cap = v->cap * 2;
        }

        int32_t *new_data = realloc(v->data, new_cap * sizeof(int32_t));
        if (new_data == NULL) {
            LOG_ERROR("intvec_push: realloc failed (cap=%zu)", new_cap);
            return ERR_OOM;
            // v->data is still valid — realloc does not free on failure
        }

        v->data = new_data;
        v->cap  = new_cap;
    }

    v->data[v->len++] = value;
    return ERR_OK;
}

void intvec_free(IntVec *v) {

    if (v == NULL) return;
    // defensive: tolerate NULL so callers don't have to guard every free

    free(v->data);
    v->data = NULL;   // prevent use-after-free
    v->len  = 0;
    v->cap  = 0;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(void) {

    // assertions
    int x = 42;
    assert(x == 42);
    REQUIRE(x > 0);

    // error handling
    int32_t result = 0;
    ErrorCode err;

    err = safe_divide(10, 0, &result);
    if (err != ERR_OK) {
        LOG_WARN("divide failed: %s", error_string(err));
    }

    err = safe_add(INT32_MAX, 1, &result);
    if (err != ERR_OK) {
        LOG_WARN("add failed: %s", error_string(err));
    }

    // input validation
    size_t name_len = 0;
    err = validate_name("Sebastian", &name_len);
    if (err == ERR_OK) {
        LOG_INFO("name valid, len=%zu", name_len);
    }

    err = validate_name(NULL, NULL);
    if (err != ERR_OK) {
        LOG_ERROR("validate_name failed: %s", error_string(err));
    }

    // defensive vec usage
    IntVec v = { .data = NULL, .len = 0, .cap = 0 };
    // designated initializer — explicitly zero all fields

    for (int i = 0; i < 10; i++) {
        err = intvec_push(&v, i * i);
        if (err != ERR_OK) {
            LOG_ERROR("push failed at i=%d: %s", i, error_string(err));
            intvec_free(&v);
            return 1;
        }
    }

    LOG_INFO("vec len=%zu cap=%zu", v.len, v.cap);
    intvec_free(&v);

    return 0;
}
