cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME GiraffeChess)
project(${TARGET_NAME})

# Set custom output directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Add the executable target
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

# Compiler flags for different platforms
if (WIN32)
	target_compile_options(${TARGET_NAME} PRIVATE /std:c++latest /W4)
elseif (UNIX)
	target_compile_options(${TARGET_NAME} PRIVATE -std=c++2b -Wall -Wextra -pedantic)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/cpputils)
target_link_libraries(${TARGET_NAME} cpputils)
