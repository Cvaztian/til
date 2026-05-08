#include <iostream>
#include <memory>
#include <cstdio>

// ── Classes — structs with behavior and access control ───────────────────────

// in C: struct holds data, free functions operate on it
// in C++: class bundles data and the functions that operate on it together

class Counter {
private:
    // private: only accessible from within the class
    int value;
    int min;
    int max;

public:
    // public: accessible from anywhere

    Counter(int min, int max)
        : value(min), min(min), max(max)
    {}
    // constructor — called automatically when the object is created
    // initializer list (: value(min), ...) initializes members before the body runs
    // prefer it over assignment in the body — more efficient for non-trivial types

    ~Counter() {
        // destructor — called automatically when the object goes out of scope
        // or when delete is called on a heap-allocated instance
        // if the class owns resources (file, memory), release them here
    }

    bool increment() {
        if (value >= max) return false;
        value++;
        return true;
    }

    int get() const {
        // const after () — this method does not modify the object
        // the compiler enforces it: any mutation inside is a compile error
        // callers holding a const Counter & can only call const methods
        return value;
    }
};

// struct in C++ is identical to class — the only difference is default access:
// struct: members are public by default
// class:  members are private by default
// convention: use struct for passive data, class for objects with behavior

// ── RAII — replaces goto cleanup ─────────────────────────────────────────────

// C pattern: manually track what was acquired and jump to cleanup on error
// C++ pattern: tie resource lifetime to object lifetime — cleanup is automatic

class FileHandle {
    FILE *f;

public:
    explicit FileHandle(const char *path)
        : f(fopen(path, "r"))
    {}
    // explicit: prevents implicit conversion from const char* to FileHandle
    // without it: FileHandle fh = "data.txt"; would silently compile

    ~FileHandle() {
        if (f != nullptr) fclose(f);
        // runs automatically when FileHandle goes out of scope
        // even if an early return or exception fires — no goto needed
    }

    bool valid() const { return f != nullptr; }

    FILE *get() const { return f; }

    // disable copy — a file handle has unique ownership
    FileHandle(const FileHandle &)            = delete;
    FileHandle &operator=(const FileHandle &) = delete;
    // = delete tells the compiler to reject any attempt to copy this object
    // without this, the compiler generates a copy that would fclose the same FILE* twice
};

void demo_raii() {

    FileHandle fh("/etc/hostname");

    if (!fh.valid()) {
        std::cerr << "could not open file\n";
        return;
        // fh's destructor still runs here — fclose is called automatically
    }

    // use fh ...
    // when this function returns (any path), ~FileHandle() runs
    // no goto, no manual cleanup
}

// ── new / delete — heap allocation with constructors ─────────────────────────

void demo_new_delete() {

    Counter *c = new Counter(0, 10);
    // new: allocates on the heap AND calls the constructor
    // malloc would allocate raw bytes — the constructor would never run

    c->increment();

    delete c;
    // delete: calls the destructor AND frees the memory
    // free() would leak — the destructor would never run

    c = nullptr;
    // null after delete — same defensive pattern as in C

    // in modern C++: prefer smart pointers over raw new/delete
    // new/delete shown here to explain the underlying mechanism
}

// ── Smart pointers — replace raw owning pointers ──────────────────────────────

void demo_unique_ptr() {

    auto c = std::make_unique<Counter>(0, 10);
    // unique_ptr: sole owner of the Counter
    // when c goes out of scope, delete is called automatically
    // no manual new/delete needed

    c->increment();
    // -> works the same as with a raw pointer

    // unique_ptr cannot be copied — ownership is unique
    // auto c2 = c;  — compile error
    auto c2 = std::move(c);
    // move transfers ownership: c is now null, c2 owns the Counter
    // this is explicit — accidental copies are rejected at compile time

    (void)c2;
}   // c2 goes out of scope here — Counter is deleted automatically

void demo_shared_ptr() {

    auto a = std::make_shared<Counter>(0, 10);
    // shared_ptr: reference-counted ownership
    // the Counter lives as long as at least one shared_ptr points to it

    auto b = a;
    // copy is allowed — both a and b point to the same Counter
    // reference count is now 2

    a.reset();
    // a releases its ownership — count drops to 1, Counter still lives

}   // b goes out of scope — count drops to 0 — Counter is deleted

// when to use which:
// unique_ptr — default choice: single owner, zero overhead over raw pointer
// shared_ptr — when ownership is genuinely shared across multiple owners
// raw pointer — for non-owning references (observing, not owning)

// ── main ──────────────────────────────────────────────────────────────────────

int main() {

    Counter c(0, 5);
    // stack-allocated — no new, no delete, no malloc
    while (c.increment()) {
        std::cout << c.get() << "\n";
    }

    demo_raii();
    demo_new_delete();
    demo_unique_ptr();
    demo_shared_ptr();

    return 0;
}
