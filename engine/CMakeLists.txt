cmake_minimum_required(VERSION 3.15) #Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

set(TARGET_NAME engine)
project(${TARGET_NAME} LANGUAGES CXX)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "")

add_library(${TARGET_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(cpputils PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

# Compiler flags for different platforms
if (MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE
		$<$<CONFIG:Debug>:/JMC>
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/GS- /O2>
		/std:c++latest /W4 /MP /utf-8 /Zi /Gy
		/wd4996
	)

	target_link_options(${TARGET_NAME} PRIVATE
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/OPT:REF /OPT:ICF>
		$<$<CONFIG:Debug>:/INCREMENTAL>
		/DEBUG:FASTLINK
	)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
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
		-Wno-missing-include-dirs -Wno-undef -Wno-switch
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:-O3>
	)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${TARGET_NAME} PRIVATE -fconcepts)
	endif()

endif()

target_link_libraries(${TARGET_NAME} cpputils)
