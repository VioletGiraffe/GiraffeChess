cmake_minimum_required(VERSION 3.15) # Must be 3.15+ because of this: https://discourse.cmake.org/t/how-to-set-warning-level-correctly-in-modern-cmake/1103
project(GiraffeChess LANGUAGES CXX)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)

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

add_subdirectory(cpputils)

# This repository's additions to cpputils_build_options
add_library(giraffe_extra_build_options INTERFACE)

if (MSVC)
	target_compile_options(giraffe_extra_build_options INTERFACE
		$<$<OR:$<CONFIG:RelWithDebInfo>,$<CONFIG:Release>>:/GS->
		/utf-8
		/wd4996
	)
	target_link_options(giraffe_extra_build_options INTERFACE /DEBUG:FASTLINK)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
	target_compile_options(giraffe_extra_build_options INTERFACE -g -Wno-switch)
endif()

# Linked by the first-party targets
# Has no options of its own: CMake emits a target's own options before those of its dependencies
# Links the additions after cpputils_build_options: their -Wno-* and /wd* must follow the shared -Wall and /W4
add_library(giraffe_build_options INTERFACE)
target_link_libraries(giraffe_build_options INTERFACE cpputils_build_options giraffe_extra_build_options)

add_subdirectory(engine)
add_subdirectory(GiraffeChess)

enable_testing()
add_subdirectory(test)
