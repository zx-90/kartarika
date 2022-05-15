cmake_minimum_required(VERSION 3.8)

set(UTIL_NAME kar)

set(kartarika_SRC

src/main.c

)

add_executable(${UTIL_NAME} ${kartarika_SRC})

if (WIN32)
target_link_libraries(${UTIL_NAME} engine shlwapi)
elseif(UNIX)
target_link_libraries(${UTIL_NAME} engine)
endif()

add_custom_command(
		TARGET ${UTIL_NAME} POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
		COMMAND ${CMAKE_COMMAND} -E copy ${ROOT_DIR}/third_party/lib/LLVM-C.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LLVM-C.dll
	)

target_include_directories(${UTIL_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
