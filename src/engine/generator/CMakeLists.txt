cmake_minimum_required(VERSION 3.8)

set(lib_generator_SRC

src/generator.c

)

add_library(generator STATIC ${lib_generator_SRC})

#find_package(LLVM 9.0 REQUIRED)
#target_link_libraries(generator LLVM) 

target_link_libraries(generator core)

target_include_directories(generator PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
