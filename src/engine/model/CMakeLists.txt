cmake_minimum_required(VERSION 3.8)

set(lib_model_SRC

src/token_type.c
src/token.c

src/module_error.c
src/module.c

)

add_library(model STATIC ${lib_model_SRC})

target_link_libraries(model core)

target_include_directories(model PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
