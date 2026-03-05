# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A from-scratch implementation of `std::vector<T>` in C++23, exploring manual memory management, exception safety, and STL internals. The implementation lives entirely in `include/vector.hpp` under the `dev` namespace as an INTERFACE (header-only) library. Tests use DocTest via vcpkg.

## Build Commands

```bash
# Configure and build (dev preset: Debug + tests enabled)
cmake --preset dev
cmake --build --preset dev

# Run all tests
ctest --preset dev

# Run a single test by name
ctest --test-dir build/dev -R "vector - scenario name"

# Production build (tests excluded)
cmake --preset release
cmake --build --preset release
```

The `test` preset builds with `RelWithDebInfo` and tests on — used in CI. The `dev` preset uses `Debug` and is for local development. The `release` preset uses `Release` with tests compiled out. Platform-specific compilers are selected automatically (GCC on Linux, Clang on macOS, MSVC on Windows).

## Code Formatting

```bash
clang-format -i include/vector.hpp tests/*.cpp
```

LLVM style, 2-space indentation, 80-char line length, left pointer alignment. Config in [.clang-format](.clang-format).

## Architecture

- **[include/vector.hpp](include/vector.hpp)**: The entire `dev::vector<T>` implementation. Currently a skeleton.
- **[tests/vector.test.cpp](tests/vector.test.cpp)**: Functional tests for the public API.
- **[tests/main.cpp](tests/main.cpp)**: DocTest main entry point (`DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN`).
- **[CMakeLists.txt](CMakeLists.txt)**: Defines the `vector` INTERFACE target and `vector_test` executable. The `VECTOR_ENABLE_TEST` option gates DocTest inclusion.

Tests are **functional** (public API) in `tests/`. DocTest is conditionally compiled via `#ifdef VECTOR_ENABLE_TEST`.

## Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes/Structs | PascalCase | `Vector`, `AllocGuard` |
| Functions/Variables | snake_case | `push_back()`, `m_size` |
| Member variables | `m_` prefix | `m_data`, `m_capacity` |
| Static members | `m_` prefix | `m_instance_count` |
| Global variables | `g_` prefix | `g_debug_mode` |
| Constants | SCREAMING_SNAKE_CASE | `MAX_SIZE` |
| Namespaces | snake_case | `dev` |
| Type aliases | PascalCase | `using SizeType = ...` |
| Trait interfaces | `-able` suffix | `Copyable` |
| Service interfaces | `I` prefix | `IAllocator` |
| Error types | `*Error` suffix | `AllocationError` |
| Files | snake_case | `vector.hpp` |

Full rationale in [docs/naming_conventions.md](docs/naming_conventions.md).

## Testing Conventions

Test naming: `TEST_CASE("vector - scenario description")`

Tests follow the AAA (Arrange-Act-Assert) pattern. Exception testing uses `CHECK_THROWS_AS` and `CHECK_THROWS_WITH`.

```cpp
TEST_CASE("vector - push_back increases size") {
  dev::vector<int> v;

  v.push_back(42);

  CHECK(v.size() == 1);
}
```

## Header Inclusion Order

Within each file, include in this order (alphabetically sorted within each group, separated by blank lines):

1. First-party headers (`#include "vector.hpp"`)
2. Third-party headers (`#include <doctest/doctest.h>`)
3. Standard library (`#include <algorithm>`)

No implicit or transitive dependencies — all dependencies must be explicitly included.
