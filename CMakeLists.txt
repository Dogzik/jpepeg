cmake_minimum_required(VERSION 3.17)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(jpepeg
        main.cpp
        huffman/huffman-decoding.cpp huffman/huffman-encoding.cpp huffman/huffman-common.cpp
        transcoder/encoding.cpp
        common-utils.cpp numeric/numeric-encoding.cpp numeric/numeric-encoding.h transcoder/processing.cpp transcoder/processing.h numeric/numeric-decoding.cpp numeric/numeric-decoding.cpp numeric/numeric-decoding.h transcoder/decoding.cpp transcoder/decoding.h)
