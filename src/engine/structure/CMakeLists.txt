cmake_minimum_required(VERSION 3.8)

set(lib_structure_SRC

src/token_type.c
src/token.c

src/module_error.c
src/module.c

)

add_library(structure STATIC ${lib_structure_SRC})

target_link_libraries(structure core)

target_include_directories(structure PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
