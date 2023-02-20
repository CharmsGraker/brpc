cmake_minimum_required(VERSION 3.21)
project(stub_generator)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(generator main.cpp code_generator.h clazz.h clazz.cpp)
