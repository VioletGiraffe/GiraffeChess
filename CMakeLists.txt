cmake_minimum_required(VERSION 3.15) #Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103
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
	target_compile_options(${TARGET_NAME} PRIVATE /std:c++latest /W4 /JMC)
elseif (UNIX)
	target_compile_options(${TARGET_NAME} PRIVATE
		-std=c++2b
		-fconcepts
		-pedantic-errors
		-Wall -Wextra -Wdelete-non-virtual-dtor -Werror=duplicated-cond
		-Werror=duplicated-branches -Warith-conversion -Warray-bounds
		-Wattributes -Wcast-align -Wcast-qual -Wconversion -Wdate-time
		-Wduplicated-branches -Wendif-labels -Werror=overflow
		-Werror=return-type -Werror=shift-count-overflow -Werror=sign-promo
		-Werror=undef -Wextra -Winit-self -Wlogical-op -Wmissing-include-dirs
		-Wnull-dereference -Wpedantic -Wpointer-arith -Wredundant-decls
		-Wshadow -Wstrict-aliasing -Wstrict-aliasing=3 -Wuninitialized
		-Wunused-const-variable=2 -Wwrite-strings -Wlogical-op
		-Wno-missing-include-dirs -Wno-undef
	)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/cpputils)
target_link_libraries(${TARGET_NAME} cpputils)
