add_executable(GiraffeChess main.cpp)

target_link_libraries(GiraffeChess PRIVATE engine cpputils giraffe_build_options)
