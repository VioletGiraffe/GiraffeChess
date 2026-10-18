cmake_minimum_required(VERSION 3.15) # Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103
project(GiraffeChess LANGUAGES CXX)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")

# Compiler and linker options shared by the first-party targets; cpputils carries its own
add_library(giraffe_build_options INTERFACE)

if (MSVC)
	target_compile_options(giraffe_build_options INTERFACE
		$<$<CONFIG:Debug>:/Ob2>
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/GS- /O2>
		/std:c++latest /W4 /MP /utf-8 /Zi /Gy
		/wd4996
	)

	target_link_options(giraffe_build_options INTERFACE
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/OPT:REF /OPT:ICF>
		$<$<CONFIG:Debug>:/INCREMENTAL>
		/DEBUG:FASTLINK
	)

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(giraffe_build_options INTERFACE
		-std=c++2b
		-pedantic-errors
		-g
		-Wall -Wextra -Wdelete-non-virtual-dtor -Werror=duplicated-cond
		-Werror=duplicated-branches -Warith-conversion -Warray-bounds
		-Wattributes -Wcast-align -Wcast-qual -Wdate-time
		-Wendif-labels -Werror=overflow
		-Werror=return-type -Werror=shift-count-overflow -Werror=sign-promo
		-Werror=undef -Winit-self -Wlogical-op -Wmissing-include-dirs
		-Wnull-dereference -Wpedantic -Wpointer-arith -Wredundant-decls
		-Wshadow -Wstrict-aliasing -Wstrict-aliasing=3 -Wuninitialized
		-Wunused-const-variable=2 -Wwrite-strings
		-Wno-missing-include-dirs -Wno-undef -Wno-switch
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:-O3>
	)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(giraffe_build_options INTERFACE -fconcepts)
	endif()

endif()

add_subdirectory(cpputils)
add_subdirectory(engine)
add_subdirectory(GiraffeChess)

enable_testing()
add_subdirectory(test)
