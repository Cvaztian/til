# Error Handling in Go
> **Tags:** `go`, `errors`, `idioms`, `fundamentals`
> **Date:** 2026-04-30
> **Related:** [modules-and-packages](modules-and-packages.md)

Go functions return `(value, error)` — both the result and the failure reason, together. No global `errno`, no sentinel return values.

```go
file, err := os.Open("file.txt")
if err != nil {
    return err  // or handle it
}
// use file safely here
```

The `error` type is an interface with one method: `Error() string`. If `err == nil`, it worked.

## vs C errno

In C, you return `-1` and check a global `errno`. The error is disconnected from the call and easy to miss or overwrite:

```c
int fd = open("file.txt", O_RDONLY);
if (fd == -1) { perror("open failed"); }
```

In Go the error is local, explicit, and part of the return type — the compiler forces you to handle or explicitly discard it.

## Discarding values

When you don't need one of the return values, use `_`:

```go
_, err := fmt.Fprintf(w, "hello")  // don't care about bytes written
```

## The bail-out pattern

When a function can fail at multiple steps, return a zero-value struct and the error at each failure point:

```go
func Collect() (SystemStats, error) {
    cpu, err := getCPU()
    if err != nil {
        return SystemStats{}, err
    }
    // ...
    return SystemStats{CPUPercent: cpu}, nil
}
```

`nil` as the error means success. Callers always check the second return value first.
