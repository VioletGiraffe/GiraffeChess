add_executable(perft_test perft_test.cpp)

target_link_libraries(perft_test PRIVATE engine cpputils giraffe_build_options)
target_compile_definitions(perft_test PRIVATE PERFT_EPD_PATH="${CMAKE_CURRENT_SOURCE_DIR}/standard.epd")

add_test(NAME perft_quick COMMAND perft_test --max-depth 5)
add_test(NAME perft_full COMMAND perft_test)
set_tests_properties(perft_quick PROPERTIES LABELS quick)
set_tests_properties(perft_full PROPERTIES LABELS full)
