cmake_minimum_required(VERSION 3.15)
project(grammar-flow CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# ── Compiler warnings ─────────────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/core/grammar.cpp
    src/io/parser.cpp
    src/io/display.cpp
    src/analysis/validator.cpp
    src/analysis/first_set.cpp
    src/analysis/follow_set.cpp
)

include_directories(src)
include_directories(src/core)
include_directories(src/io)
include_directories(src/analysis)

add_executable(grammar-flow ${SOURCES})

# ── Output directory ──────────────────────────────────────────────────────────
set_target_properties(grammar-flow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
)
