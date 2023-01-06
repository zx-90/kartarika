cmake_minimum_required(VERSION 3.8)

set(lib_lexer_SRC

src/lexer.c
src/first_lexer.c
src/second_lexer.c
src/check_alphabet.c

)

add_library(lexer STATIC ${lib_lexer_SRC})

target_link_libraries(lexer structure core)

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
