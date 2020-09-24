cmake_minimum_required(VERSION 3.8)

set(lib_engine_SRC

src/engine.c

)

add_library(engine STATIC ${lib_engine_SRC})

target_link_libraries(engine core)
target_link_libraries(engine lexer)
target_link_libraries(engine parser)
target_link_libraries(engine analyzer)
target_link_libraries(engine generator)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
