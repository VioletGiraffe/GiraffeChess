cmake_minimum_required(VERSION 3.15) #Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(TARGET_NAME test)
project(${TARGET_NAME})

# Set custom output directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "")

# Add the executable target
#add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})
add_executable(${TARGET_NAME} perft_test.cpp)

# Compiler flags for different platforms
if (WIN32)
	target_compile_options(${TARGET_NAME} PRIVATE
		$<$<CONFIG:Debug>:/JMC>
		$<$<CONFIG:RelWithDebInfo>:/GS- /O2>
		/std:c++latest /W4 /MP /utf-8 /Zi /Gy
		/wd4996
	)

	target_link_options(${TARGET_NAME} PRIVATE
		$<$<CONFIG:RelWithDebInfo>:/OPT:REF /OPT:ICF>
		$<$<CONFIG:Debug>:/INCREMENTAL>
		/DEBUG:FASTLINK
	)

elseif (UNIX)
	target_compile_options(${TARGET_NAME} PRIVATE
		-std=c++2b
		-pedantic-errors
		-Wall -Wextra -Wdelete-non-virtual-dtor -Werror=duplicated-cond
		-Werror=duplicated-branches -Warith-conversion -Warray-bounds
		-Wattributes -Wcast-align -Wcast-qual -Wdate-time
		-Wduplicated-branches -Wendif-labels -Werror=overflow
		-Werror=return-type -Werror=shift-count-overflow -Werror=sign-promo
		-Werror=undef -Wextra -Winit-self -Wlogical-op -Wmissing-include-dirs
		-Wnull-dereference -Wpedantic -Wpointer-arith -Wredundant-decls
		-Wshadow -Wstrict-aliasing -Wstrict-aliasing=3 -Wuninitialized
		-Wunused-const-variable=2 -Wwrite-strings -Wlogical-op
		-Wno-missing-include-dirs -Wno-undef
		$<$<CONFIG:RelWithDebInfo>:-O3>
	)

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${TARGET_NAME} PRIVATE -fconcepts)
	endif()

endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/../cpputils ${CMAKE_BINARY_DIR}/cpputils)
add_subdirectory(${CMAKE_SOURCE_DIR}/../engine ${CMAKE_BINARY_DIR}/engine)

target_link_libraries(${TARGET_NAME} cpputils)
target_link_libraries(${TARGET_NAME} engine)
