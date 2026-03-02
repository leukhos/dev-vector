# dev::vector

A from-scratch implementation of `std::vector<T>` in C++, exploring manual memory management, exception safety, and STL internals. Inspired by Quasar Chunawala's [Overload article](https://accu.org/journals/overload/34/191/chunawala/) — the article can serve as a useful guide and reference, but this implementation does not strictly follow its code.

## Goals

- Understand how `std::vector` works under the hood
- Practice manual memory management with aligned allocation via `operator new`
- Apply the Rule of Five (copy/move constructors and assignment operators)
- Build exception-safe code using smart pointers and uninitialized algorithms
- Follow a test-driven approach with DocTest

## Topics Covered

- **Core data structure**: raw data pointer, size, and capacity
- **Constructors**: default, copy, move, parameterized, and `initializer_list`
- **Memory management**: aligned allocation, custom deleters, `unique_ptr` for cleanup
- **Key operations**: `reserve()`, `resize()`, `push_back()`, `emplace_back()`, `insert()`
- **Exception safety**: strong guarantees using uninitialized algorithms that clean up on failure
- **Iterator support**: begin/end iterators with range-based for loop compatibility

## Project Structure

```
dev-vector/
├── CMakeLists.txt              # Main CMake configuration
├── CMakePresets.json           # CMake presets (dev, test, prod)
├── vcpkg.json                  # vcpkg dependencies manifest
├── vcpkg-configuration.json    # vcpkg configuration
├── include/
│   └── vector.hpp              # Header-only vector<T> implementation (dev namespace)
├── tests/
│   ├── main.cpp                # DocTest main entry point
│   └── vector.test.cpp         # Unit and functional tests
├── cmake/
│   └── presets/                # Platform-specific preset files
└── docs/
    ├── code_guidelines.md      # Coding standards
    └── naming_conventions.md   # Naming conventions
```

The implementation lives entirely in `include/vector.hpp` under the `dev` namespace, as an INTERFACE library (header-only).

## Building

The project uses purpose-based CMake presets with automatic platform detection.

| Preset | Build Type      | Tests | Description                              |
|--------|-----------------|-------|------------------------------------------|
| `dev`  | Debug           | ON    | Development with full debug symbols      |
| `test` | RelWithDebInfo  | ON    | Test execution with optimizations        |
| `prod` | Release         | OFF   | Production build, tests compiled out     |

```bash
# Development (debug + tests)
cmake --preset dev
cmake --build build
ctest --test-dir build

# Production release
cmake --preset prod
cmake --build build
```

**Platform-Specific Compilers:**
- **Linux**: GCC with `-Wall -Wextra -Wpedantic`
- **macOS**: Clang with `-Wall -Wextra -Wpedantic`
- **Windows**: MSVC with `/W4 /permissive- /EHsc`

## CMake Options

- `VECTOR_ENABLE_TEST`: Enable/disable building tests (default: OFF)

## Dependencies

- **DocTest**: Lightweight, header-only testing framework (vcpkg `test` feature, enabled with `VECTOR_ENABLE_TEST=ON`)

## Code Guidelines

- **Naming Conventions**: See [docs/naming_conventions.md](docs/naming_conventions.md)
- **Code Formatting**: See [docs/code_guidelines.md](docs/code_guidelines.md)
- **Namespace**: All code lives under `dev::`
- **Tests**: Functional tests in `tests/`, following AAA pattern with `TEST_CASE("vector - scenario")` naming

## Reference

Quasar Chunawala, *"Implementing vector\<T\>"*, ACCU Overload 191 (2025) — [accu.org/journals/overload/34/191/chunawala/](https://accu.org/journals/overload/34/191/chunawala/)

This project is inspired by the article and it can serve as a companion or guide while reading it, but the code here is independently written and may differ in approach, structure, and detail.

## License

MIT License - see LICENSE file for details.
