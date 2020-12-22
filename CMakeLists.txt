cmake_minimum_required(VERSION 3.17)
project(lab2)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(jpepeg main.cpp huffman/huffman-decoding.cpp coder/encoding.cpp coder/encoding.h)
