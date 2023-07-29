cmake_minimum_required(VERSION 3.8)

set(lib_generator_SRC

    src/llvm_function.c
    src/llvm_data.c

    src/generator.c

)

add_library(generator STATIC ${lib_generator_SRC})

# Здесь надо подключить библиотеку LLVM и для Windows тоже.
if (UNIX)
find_package(LLVM 9.0 REQUIRED)
target_link_libraries(generator LLVM) 
endif()
if (WIN32)
target_include_directories(generator PUBLIC ${ROOT_DIR}/third_party/include)
target_link_libraries(generator ${ROOT_DIR}/third_party/lib/LLVM-C.lib)
endif()

target_link_libraries(generator model core)

target_include_directories(generator PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
