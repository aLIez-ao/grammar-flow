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

# ── Vinculación Estática (Para evitar errores de DLL en Windows) ──────────────
if(WIN32 AND NOT MSVC)
    # Esto incluye libstdc++ y libgcc dentro del .exe
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
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

add_executable(grammar-flow ${SOURCES})

# ── Includes ──────────────────────────────────────────────────────────────────
target_include_directories(grammar-flow PRIVATE src)

# ── Output directory ──────────────────────────────────────────────────────────
set_target_properties(grammar-flow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)