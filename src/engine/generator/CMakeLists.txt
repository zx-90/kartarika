cmake_minimum_required(VERSION 3.8)

set(lib_generator_SRC

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
add_custom_command(
		TARGET generator POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_DIR}/third_party/lib/LLVM-C.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LLVM-C.dll
	)
endif()

target_link_libraries(generator core)

target_include_directories(generator PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
