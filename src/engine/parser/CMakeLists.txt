cmake_minimum_required(VERSION 3.8)

set(lib_parser_SRC

src/parser.c

src/structure/_structure.c
src/structure/blocks.c
src/structure/brackets.c
src/structure/lines.c

src/expression/_expression.c
src/expression/path_call.c
src/expression/space.c
src/expression/method_arguments.c
src/expression/operand.c
src/expression/after.c

src/root/_root.c
src/root/variable.c
src/root/method.c

src/algorithm/_algorithm.c
src/algorithm/expression.c
src/algorithm/return.c
src/algorithm/declaration.c
src/algorithm/assign.c
src/algorithm/block.c
src/algorithm/pass.c
src/algorithm/clean.c
src/algorithm/if.c
src/algorithm/while.c
src/algorithm/break.c

src/base/check.c

)

add_library(parser STATIC ${lib_parser_SRC})

target_link_libraries(parser model core)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
