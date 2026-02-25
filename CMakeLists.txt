cmake_minimum_required(VERSION 3.30)
project(calculator)

# Main library
add_library(calculator STATIC)
add_library(calculator::calculator ALIAS calculator)

target_sources(calculator
    PUBLIC
        FILE_SET calculator_headers
        TYPE HEADERS
        BASE_DIRS include
        FILES
            include/calculator.hpp
    PRIVATE
        src/calculator.cpp
)

target_compile_features(calculator PUBLIC cxx_std_20)

install(
    TARGETS calculator
    EXPORT calculatorTargets
    FILE_SET calculator_headers
)

install(
    EXPORT calculatorTargets
    DESTINATION share/cmake/calculator
    NAMESPACE calculator::
)

install(
    FILES cmake/calculatorConfig.cmake
    DESTINATION share/cmake/calculator
)

option(CALCULATOR_ENABLE_TEST "Enable testing" OFF)
if(CALCULATOR_ENABLE_TEST)
    find_package(doctest REQUIRED)
    find_package(trompeloeil REQUIRED)

    target_compile_definitions(calculator PRIVATE CALCULATOR_ENABLE_TEST)
    target_link_libraries(calculator PRIVATE doctest::doctest trompeloeil::trompeloeil)

    include(CTest)

    add_executable(calculator_tests)

    target_sources(calculator_tests 
        PRIVATE
            tests/main.cpp
            tests/calculator.test.cpp
            $<TARGET_OBJECTS:calculator>
    )

    target_link_libraries(calculator_tests
        PRIVATE
            calculator
            doctest::doctest
    )

    include(doctest)
    doctest_discover_tests(calculator_tests)
endif()


option(CALCULATOR_ENABLE_BENCH "Enable benchmarking" OFF)
if(CALCULATOR_ENABLE_BENCH)
    find_package(benchmark REQUIRED)

add_executable(calculator_benchmarks)

target_sources(calculator_benchmarks
    PRIVATE
        benches/calculator.benchmark.cpp
)

target_link_libraries(calculator_benchmarks
    PRIVATE
        calculator
        benchmark::benchmark
)

endif()