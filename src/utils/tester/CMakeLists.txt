cmake_minimum_required(VERSION 3.8)

set(UTIL_NAME tester)

set(tester_SRC

src/main.c
src/test_suite.c
src/test.c

)

add_executable(${UTIL_NAME} ${tester_SRC})

target_link_libraries(${UTIL_NAME} engine shlwapi)
#target_link_libraries(${UTIL_NAME} stdc++fs)

target_include_directories(${UTIL_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

add_custom_command(TARGET ${UTIL_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/тесты ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/тесты)
