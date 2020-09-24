cmake_minimum_required(VERSION 3.8)

set(lib_core_SRC

src/string_builder.c
src/stream.c
src/string.c
src/file_system.c
src/error.c

src/cursor.c
src/module_error.c
src/stream_cursor.c
src/module.c
src/token.c
src/token_type.c

)

add_library(core STATIC ${lib_core_SRC})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
