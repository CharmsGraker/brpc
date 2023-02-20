cmake_minimum_required(VERSION 3.21)
project(stub_generator)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(generator main.cpp utils.cpp code_generator.h code_analyser/parse_object/clazz.cpp code_analyser/parse_object/constant.cpp code_analyser/classparser.cpp code_analyser/common.cpp)
add_executable(test_headerparser test/test_headerparser.cpp code_analyser/parse_object/clazz.cpp code_analyser/common.cpp code_analyser/headerparser.cpp code_analyser/parse_object/constant.h code_analyser/parse_object/constant.cpp utils.cpp)
