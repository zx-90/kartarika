cmake_minimum_required(VERSION 3.8)

set(lib_parser_SRC

src/parser.c

src/structure/blocks.c
src/structure/brackets.c
src/structure/lines.c
src/structure/space.c

src/arithmetic/path.c

src/root/variable.c
src/root/method/header.c
src/root/method/return.c

)

add_library(parser STATIC ${lib_parser_SRC})

target_link_libraries(parser core)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
