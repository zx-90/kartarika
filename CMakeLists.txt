cmake_minimum_required(VERSION 3.8)

project(kartarika VERSION 0.1.1.0 LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(MSVC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin)
endif()

# TODO: �������� �������� �� �� �������� ���� �����������.  
if (UNIX)
	add_compile_options(-Werror -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wconversion -Wenum-compare -Wfloat-equal -Wredundant-decls -Wsign-conversion)
elseif(WIN32)
	# TODO: �������� ����� ����������� ��� ����� ������� �������� ����.
	add_compile_options(/Y-)
endif()

add_subdirectory(src/engine/core)
add_subdirectory(src/engine/model)
add_subdirectory(src/engine/lexer)
add_subdirectory(src/engine/parser)
add_subdirectory(src/engine/analyzer)
add_subdirectory(src/engine/generator)
add_subdirectory(src/engine/engine)

add_subdirectory(src/utils/kar)
add_subdirectory(src/utils/tester)
