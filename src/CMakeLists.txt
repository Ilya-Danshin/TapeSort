cmake_minimum_required(VERSION 3.22)
project(src)

set(CMAKE_CXX_STANDARD 17)

add_executable(src
        main.cpp
        Tape/ITape.h Tape/Tape.h Tape/Tape.cpp
        Sort/ISort.h Sort/Sort.h Sort/Sort.cpp
        )
