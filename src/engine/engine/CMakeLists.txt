cmake_minimum_required(VERSION 3.8)

set(lib_engine_SRC

src/engine.c

)

add_library(engine STATIC ${lib_engine_SRC})

target_link_libraries(engine core)
target_link_libraries(engine model)
target_link_libraries(engine lexer)
target_link_libraries(engine parser)
target_link_libraries(engine analyzer)
target_link_libraries(engine generator)

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

add_custom_command(TARGET engine POST_BUILD COMMAND ${CMAKE_C_COMPILER} -c ${CMAKE_SOURCE_DIR}/src/kar/library.c -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/library.o)
