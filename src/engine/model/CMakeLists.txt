cmake_minimum_required(VERSION 3.8)

set(lib_model_SRC

src/token_type.c
src/token.c
src/module.c

src/project_error.c
src/project_error_list.c

src/vartree_function_params.c
src/vartree.c

src/local_vars.c

src/vars.c
src/project.c
)

add_library(model STATIC ${lib_model_SRC})

target_link_libraries(model core)

target_include_directories(model PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
