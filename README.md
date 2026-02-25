# C++ Project Template

A modern C++ project template with CMake build system, vcpkg dependency management, DocTest testing framework, and Google Benchmark performance testing.


## Features

- **CMake Build System**: Modern CMake (3.30+) with FILE_SET support and target-based configuration
- **Optional vcpkg Integration**: CMake presets provide optional vcpkg dependency management with manifest mode - the project is fully independent of vcpkg
- **Testing**: DocTest framework with embedded unit tests and separate functional tests
- **Benchmarking**: Google Benchmark in `benches/` directory
- **Modern C++**: C++20 standard with comprehensive compiler warnings
- **Multi-Platform Support**: Automatic platform detection with purpose-based presets (dev, test, prod, bench)
- **Continuous Integration**: GitHub Actions CI testing across Ubuntu, macOS, and Windows with intelligent caching
- **Code Standards**: Documented coding guidelines and naming conventions
- **Development Container**: Ready-to-use devcontainer configuration based on Microsoft's official containers

## Project Structure

```
cpp-project-template/
├── CMakeLists.txt              # Main CMake configuration (includes all targets)
├── CMakePresets.json           # CMake presets (dev, test, prod, bench)
├── vcpkg.json                  # vcpkg dependencies manifest
├── vcpkg-configuration.json    # vcpkg configuration
├── include/                    # Public headers (FILE_SET)
│   └── calculator.hpp            # Example header
├── cmake/                      # CMake configuration
│   ├── presets/                # Platform-specific preset files
│   └── calculatorConfig.cmake  # Package configuration
├── src/                        # Source files
│   └── calculator.cpp          # Implementation + embedded unit tests
├── tests/                      # Functional/Integration tests
│   ├── main.cpp                # DocTest main entry point
│   └── calculator.test.cpp     # Functional tests for public API
├── benches/                    # Performance benchmarks
│   └── calculator.benchmark.cpp # snake_case benchmark functions
└── docs/                       # Documentation
    ├── code_guidelines.md      # Coding standards
    └── naming_conventions.md   # Naming conventions
```

## CMake Options

- `CALCULATOR_ENABLE_TEST`: Enable/disable building tests (default: OFF)
- `CALCULATOR_ENABLE_BENCH`: Enable/disable building benchmarks (default: OFF)

## Dependencies

The project has minimal runtime dependencies, managed through vcpkg manifest features:

- **DocTest**: Lightweight, header-only testing framework (vcpkg `test` feature, enabled with `CALCULATOR_ENABLE_TEST=ON`)
- **Trompeloeil**: Modern C++ mocking framework (vcpkg `test` feature, enabled with `CALCULATOR_ENABLE_TEST=ON`)
- **Google Benchmark**: Performance benchmarking (vcpkg `bench` feature, enabled with `CALCULATOR_ENABLE_BENCH=ON`)

Test dependencies are truly optional - they are only installed when the corresponding vcpkg manifest feature is enabled. Dependencies are loaded via CMake's `find_package()` function. The project includes optional vcpkg integration through CMake presets, but this is not required - you can use any dependency management approach you prefer.

## Code Guidelines

Coding standards are documented in the `docs/` directory:

- **Naming Conventions**: See [docs/naming_conventions.md](docs/naming_conventions.md)
- **Code Formatting**: See [docs/code_guidelines.md](docs/code_guidelines.md) for formatting and structure guidelines
- **Header Organization**: Critical header inclusion order with mandatory grouping and comments
- **Test Organization**:
  - **Unit tests**: Embedded in source files (`src/*.cpp`) - test implementation details
  - **Functional tests**: In `tests/` directory - test public API and workflows
- **Test Standards**: AAA pattern with DocTest, `TEST_CASE("Module - scenario")` naming
- **Benchmark Standards**: `benchmark_component_operation_scenario` snake_case naming

### Naming Summary

- **Classes/Structs**: `PascalCase` (Calculator, DataProcessor)
- **Variables**: `snake_case` (counter, file_name)
- **Members**: `m_` prefix (m_value, m_is_valid)
- **Functions**: `snake_case` (process_data, get_name)
- **Constants**: `SCREAMING_SNAKE_CASE` (MAX_BUFFER_SIZE)
- **Namespaces**: `snake_case` (data_processing, networking)
- **Error Types**: `PascalCaseError` (FileNotFoundError, ParseError)
- **Trait interfaces**: `-able` suffix (Drawable, Serializable)
- **Service interfaces**: `I` prefix (ILogger, ICalculator) - also for mocking
- **Files**: `snake_case.{hpp,cpp}` (calculator.hpp, data_processor.cpp)

### Code Formatting

The project uses clang-format with LLVM style (2-space indentation, 80 character line length):

```bash
clang-format -i src/**/*.{cpp,h} tests/**/*.{cpp,h} benches/**/*.cpp
```

## Using CMake Presets

The project uses purpose-based CMake presets with automatic platform detection. All presets use Ninja generator and vcpkg integration (when `VCPKG_ROOT` is set).

**Available Presets:**

| Preset | Build Type | Tests | Benchmarks | Description |
|--------|------------|-------|------------|-------------|
| `dev` | Debug | ON | OFF | Development with full debug symbols |
| `test` | RelWithDebInfo | ON | OFF | Test execution with optimizations |
| `prod` | Release | OFF | OFF | Production build, tests compiled out |
| `bench` | Release | OFF | ON | Performance benchmarking |

**Platform-Specific Compilers:**
- **Linux**: GCC with `-Wall -Wextra -Wpedantic`
- **macOS**: Clang with `-Wall -Wextra -Wpedantic`
- **Windows**: MSVC with `/W4 /permissive- /EHsc`

```bash
# Development (debug + tests)
cmake --preset dev
cmake --build build
ctest --test-dir build

# Production release
cmake --preset prod
cmake --build build

# Benchmarking
cmake --preset bench
cmake --build build
./build/benches/calculator_benchmarks
```

See `CMakePresets.json` and `cmake/presets/` for complete configuration details.

## License

MIT License - see LICENSE file for details.
