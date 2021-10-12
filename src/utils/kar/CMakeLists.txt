cmake_minimum_required(VERSION 3.8)

set(UTIL_NAME kar)

set(kartarika_SRC

src/main.c

)

add_executable(${UTIL_NAME} ${kartarika_SRC})

target_link_libraries(${UTIL_NAME} engine shlwapi)
#target_link_libraries(${UTIL_NAME} stdc++fs)

target_include_directories(${UTIL_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
