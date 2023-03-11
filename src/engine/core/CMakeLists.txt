cmake_minimum_required(VERSION 3.8)

if (WIN32)
set(lib_core_SRC_system src/file_system_win.c src/console_system_win.c src/stream_win.c)
elseif(UNIX)
set(lib_core_SRC_system src/file_system_unix.c src/console_system_unix.c src/stream_unix.c)
endif()

set(lib_core_SRC

src/string_builder.c
src/string.c
src/string_list.c
src/unicode.c
${lib_core_SRC_system}
src/error.c
src/stack.c

src/cursor.c
src/stream_cursor.c

)

add_library(core STATIC ${lib_core_SRC})

target_include_directories(core PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
