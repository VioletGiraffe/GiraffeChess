add_executable(perft_test perft_test.cpp)

target_link_libraries(perft_test PRIVATE engine cpputils giraffe_build_options)

# The test opens ../test/standard.epd relative to its working directory
add_test(NAME perft COMMAND perft_test WORKING_DIRECTORY $<TARGET_FILE_DIR:perft_test>)
