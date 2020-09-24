cmake_minimum_required(VERSION 3.8)

set(lib_parser_SRC

src/parser.c
src/elements/blocks.c
src/elements/brackets.c
src/elements/function.c
src/elements/lines.c
src/elements/path.c
src/elements/return.c
src/elements/space.c

)

add_library(parser STATIC ${lib_parser_SRC})

target_link_libraries(parser core)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
