cmake_minimum_required(VERSION 3.24)
project(tests)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

add_subdirectory(googletest)

add_executable(tests
        main.cpp
        ../src/Tape/ITape.h ../src/Tape/Tape.h ../src/Tape/Tape.cpp
        ../src/Sort/ISort.h ../src/Sort/Sort.h ../src/Sort/Sort.cpp
        )

target_link_libraries(tests gtest_main gmock_main)