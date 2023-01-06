cmake_minimum_required(VERSION 3.8)

set(lib_analyzer_SRC

src/analyzer.c

)

add_library(analyzer STATIC ${lib_analyzer_SRC})

target_link_libraries(analyzer model core)

target_include_directories(analyzer PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
