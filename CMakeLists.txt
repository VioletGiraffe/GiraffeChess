cmake_minimum_required(VERSION 3.15) # Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103
project(GiraffeChess LANGUAGES CXX)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Replace CMake's per-configuration defaults, inherited by the subdirectories; config-independent options are on giraffe_build_options
if (MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "/Od /Ob2 /RTC1 /D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<LOWER_CASE:$<CONFIG>>)

# Compiler and linker options shared by the first-party targets; cpputils carries its own
add_library(giraffe_build_options INTERFACE)

if (MSVC)
	target_compile_options(giraffe_build_options INTERFACE
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/GS->
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
	)

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(giraffe_build_options INTERFACE -fconcepts)
	endif()

endif()

if (WIN32)
	target_compile_definitions(giraffe_build_options INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

add_subdirectory(cpputils)
add_subdirectory(engine)
add_subdirectory(GiraffeChess)

enable_testing()
add_subdirectory(test)
