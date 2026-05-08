#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <algorithm>

// ── std::string — replaces char[] + string.h ─────────────────────────────────

void demo_string() {

    std::string name = "Sebastian";
    // heap-allocated, growable, null-terminated internally
    // knows its own length — no strlen needed

    std::string greeting = "Hello, " + name;
    // + concatenates — no strcpy, no strcat, no buffer size to track

    greeting += "!";
    // += appends in place

    size_t len = name.length();
    // O(1) — length is stored, not computed by scanning for '\0'

    std::string sub = name.substr(0, 4);
    // "Seba" — safe extraction, bounds checked

    bool found = name.find("bast") != std::string::npos;
    // npos is the "not found" sentinel (max value of size_t)

    // C string interop:
    const char *raw = name.c_str();
    // c_str() gives a null-terminated const char* for C API calls
    // valid as long as name is not modified

    (void)len; (void)sub; (void)found; (void)raw;
}

// ── std::vector — replaces raw arrays + malloc/realloc ───────────────────────

void demo_vector() {

    std::vector<int> v = {1, 2, 3, 4};
    // heap-allocated, growable, owns its memory
    // destructor frees it automatically — no free() needed

    v.push_back(5);
    // appends — reallocates internally if needed, doubles capacity
    // same growth strategy as the IntVec in code_quality.c, but automatic

    size_t len = v.size();
    // number of elements — always available, no separate variable to track

    int third = v[2];
    // no bounds check — same as C array indexing (fast, unsafe)

    int third_safe = v.at(2);
    // bounds-checked — throws std::out_of_range if index is invalid
    // use at() when the index comes from untrusted input

    v.pop_back();
    // removes last element

    v.clear();
    // removes all elements, size becomes 0, capacity unchanged

    // iteration
    for (const auto &val : v) {
        (void)val;
    }

    // raw pointer interop (e.g., for C APIs expecting int*):
    // v.data() returns int* to the underlying contiguous array

    (void)len; (void)third; (void)third_safe;
}

// ── std::array — replaces fixed-size C arrays ────────────────────────────────

void demo_array() {

    std::array<int, 4> arr = {1, 2, 3, 4};
    // stack-allocated, fixed size (set at compile time)
    // unlike C arrays: knows its size, works with STL algorithms, can be copied

    size_t len = arr.size();
    // 4 — unlike sizeof(arr)/sizeof(arr[0]), this just works

    arr[0] = 10;
    // no bounds check

    arr.at(0) = 10;
    // bounds-checked

    // std::array does NOT decay to a pointer when passed to a function
    // the full type (including size) is preserved
    // void f(std::array<int, 4> &a) — the compiler enforces the size

    (void)len;
}

// ── std::map / std::unordered_map ─────────────────────────────────────────────

void demo_maps() {

    std::unordered_map<std::string, int> scores;
    // hash map: O(1) average lookup — equivalent to a hash table in C
    // key: std::string, value: int

    scores["alice"] = 100;
    scores["bob"]   = 85;
    // operator[] inserts a default value (0 for int) if key doesn't exist

    scores["alice"] += 10;

    if (scores.count("alice") > 0) {
        // count() returns 0 or 1 — use to check existence without inserting
        std::cout << scores["alice"] << "\n";
    }

    auto it = scores.find("bob");
    if (it != scores.end()) {
        // find() returns an iterator — end() means not found
        std::cout << it->first << ": " << it->second << "\n";
        // it->first is the key, it->second is the value
    }

    for (const auto &[key, val] : scores) {
        // structured bindings (C++17): unpack key/value directly
        (void)key; (void)val;
    }

    std::map<std::string, int> ordered;
    // map: O(log n) lookup — keys are sorted (red-black tree internally)
    // use when you need ordered iteration; use unordered_map otherwise
    (void)ordered;
}

// ── Algorithms — STL replaces manual loops ───────────────────────────────────

void demo_algorithms() {

    std::vector<int> v = {5, 3, 1, 4, 2};

    std::sort(v.begin(), v.end());
    // sorts in place — equivalent to qsort but type-safe, no void*

    std::sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
    // lambda as comparator — descending order
    // [](int a, int b) { ... } is an anonymous function defined inline

    auto it = std::find(v.begin(), v.end(), 3);
    if (it != v.end()) {
        // it points to the element
    }

    int sum = 0;
    for (const auto &x : v) sum += x;
    // manual accumulate — std::accumulate exists but this is clearer

    // algorithms operate on iterators — they work on any container
    // std::sort on a vector, std::array, or raw array with begin/end pointers
}

// ── Templates — type-safe generics ───────────────────────────────────────────

template <typename T>
T max_of(T a, T b) {
    return (a > b) ? a : b;
}
// T is resolved at compile time — the compiler generates a separate version
// for each type it's called with (int, double, std::string, etc.)
// unlike void* in C: fully type-safe, no casting

template <typename T>
struct Pair {
    T first;
    T second;
};
// class templates: std::vector<int>, std::array<int, 4> are both template instantiations

void demo_templates() {

    int    a = max_of(3, 5);       // T = int
    double b = max_of(1.2, 3.4);   // T = double

    Pair<int>    p1 = {1, 2};
    Pair<double> p2 = {1.1, 2.2};

    (void)a; (void)b; (void)p1; (void)p2;
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {

    demo_string();
    demo_vector();
    demo_array();
    demo_maps();
    demo_algorithms();
    demo_templates();

    return 0;
}
