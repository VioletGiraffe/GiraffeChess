file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

add_library(engine STATIC ${SOURCES} ${HEADERS})

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

target_link_libraries(engine PUBLIC cpputils PRIVATE giraffe_build_options)
