#include <iostream>
#include <string>

// ── nullptr — replaces NULL ───────────────────────────────────────────────────

void demo_nullptr() {

    int *p = nullptr;
    // nullptr is a typed null pointer constant (std::nullptr_t)
    // NULL is just 0 — it can silently convert to int, causing ambiguity:
    //   void f(int);
    //   void f(int *);
    //   f(NULL);    // calls f(int) — probably not what you meant
    //   f(nullptr); // calls f(int *) — unambiguous

    if (p == nullptr) {
        // prefer nullptr in all pointer comparisons
    }

    (void)p;
}

// ── References — aliases that are not pointers ────────────────────────────────

void increment(int &n) {
    // int &n — reference parameter: n is an alias for the caller's variable
    // no pointer syntax at the call site: increment(x), not increment(&x)
    // a reference cannot be null and cannot be reseated to point elsewhere
    n++;
}

void demo_references() {

    int x = 10;

    int &ref = x;
    // ref is an alias for x — they share the same memory address
    // &ref == &x
    ref = 20;
    // x is now 20 — writing to ref writes to x

    increment(x);
    // x is now 21 — no & needed at the call site

    // when to use references vs pointers:
    // reference — when the argument is always required and never null
    // pointer   — when null is a valid state, or you need to reseat
    const int &cref = x;
    // const reference — read-only alias, common for function parameters
    // avoids a copy without allowing modification
    // void print(const std::string &s) — no copy, no mutation

    (void)ref; (void)cref;
}

// ── auto — type inference ─────────────────────────────────────────────────────

void demo_auto() {

    auto i = 42;
    // int — compiler infers the type from the initializer

    auto d = 3.14;
    // double

    auto s = std::string("hello");
    // std::string — explicit construction needed (otherwise it'd be const char*)

    // auto shines with verbose types:
    // without auto: std::string::iterator it = str.begin();
    // with auto:    auto it = str.begin();

    // auto does not mean dynamic typing — the type is fixed at compile time
    // i = "hello";  — compile error: i is int

    // use const auto & for read-only range loops to avoid copies
    // for (const auto &item : container) { ... }

    (void)i; (void)d; (void)s;
}

// ── Range-based for — replaces index loops over containers ───────────────────

void demo_range_for() {

    int arr[] = {1, 2, 3, 4, 5};

    for (int val : arr) {
        // val is a COPY of each element — safe to read, changes don't affect arr
        (void)val;
    }

    for (int &val : arr) {
        // val is a REFERENCE — changes modify arr in place
        val *= 2;
    }

    // for (const int &val : arr) — read-only reference, no copy
    // works on any type with begin()/end(): arrays, std::vector, std::string, etc.

    // C equivalent:
    // for (int i = 0; i < 5; i++) { arr[i] *= 2; }
    // range-for is cleaner and eliminates the off-by-one risk
}

// ── Namespaces — replaces C's prefix convention ──────────────────────────────

namespace math {

    int add(int a, int b) { return a + b; }
    int mul(int a, int b) { return a * b; }

    namespace trig {
        // namespaces nest
        double sin_approx(double x) { return x; }
    }
}

// in C you'd write math_add(), math_mul() to avoid collisions
// namespaces scope the names without polluting the global namespace

void demo_namespaces() {

    int r1 = math::add(1, 2);
    // :: is the scope resolution operator

    int r2 = math::mul(3, 4);

    // using math::add; — imports a single name into local scope
    // using namespace math; — imports everything (avoid in headers: pollutes callers)

    (void)r1; (void)r2;
}

// ── constexpr — replaces #define for constants ───────────────────────────────

#define MAX_C 100
// C macro: no type, no scope, no debugger visibility
// the preprocessor replaces it with 100 before compilation

constexpr int MAX_CPP = 100;
// typed, scoped, visible in the debugger
// evaluated at compile time — zero runtime cost, just like #define
// can be used in array sizes, switch cases, template arguments

constexpr int square(int x) {
    return x * x;
    // constexpr function: if called with a compile-time value,
    // the result is computed at compile time
}

void demo_constexpr() {

    int arr[MAX_CPP];
    // valid — MAX_CPP is a compile-time constant

    constexpr int s = square(5);
    // computed at compile time: s == 25, no runtime call

    static_assert(square(4) == 16, "square is wrong");
    // static_assert works because square(4) is a compile-time value

    (void)arr; (void)s;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {

    demo_nullptr();
    demo_references();
    demo_auto();
    demo_range_for();
    demo_namespaces();
    demo_constexpr();

    return 0;
}
