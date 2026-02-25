cmake_minimum_required(VERSION 3.30)
project(vector)

# Main library
add_library(vector INTERFACE)
add_library(vector::vector ALIAS vector)

target_sources(vector
    INTERFACE
        FILE_SET vector_headers
        TYPE HEADERS
        BASE_DIRS include
        FILES
            include/vector.hpp
)

target_compile_features(vector INTERFACE cxx_std_23)

option(VECTOR_ENABLE_TEST "Enable testing" OFF)
if(VECTOR_ENABLE_TEST)
    find_package(doctest REQUIRED)

    include(CTest)

    add_executable(vector_tests)

    target_sources(vector_tests 
        PRIVATE
            tests/main.cpp
            tests/vector.test.cpp
    )

    target_link_libraries(vector_tests
        PRIVATE
            vector
            doctest::doctest
    )

    include(doctest)
    doctest_discover_tests(vector_tests)
endif()
