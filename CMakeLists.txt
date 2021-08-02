cmake_minimum_required(VERSION 3.8)

project(kartarika VERSION 0.1.1.0 LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

#add_compile_options(
    #-Werror

    #-Wall
    #-Wextra
    #-Wpedantic

    #-Wcast-align
    #-Wcast-qual
    #-Wconversion
    #-Wenum-compare
    #-Wfloat-equal
    #-Wredundant-decls
    #-Wsign-conversion
#)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/engine/core)
add_subdirectory(src/engine/lexer)
add_subdirectory(src/engine/parser)
add_subdirectory(src/engine/analyzer)
add_subdirectory(src/engine/generator)
add_subdirectory(src/engine/engine)

add_subdirectory(src/utils/kar)
add_subdirectory(src/utils/tester)
