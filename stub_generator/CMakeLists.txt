cmake_minimum_required(VERSION 3.21)
project(stub_generator)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

set(PARSE_SRC code_analyser/parse_object/parse_object.cpp
        code_analyser/parse_object/clazz.cpp
        code_analyser/common.cpp
        code_analyser/parse_object/constant.cpp
        code_analyser/classparser.cpp
        code_analyser/headerparser.cpp)

add_executable(generator main.cpp utils.cpp code_generator.h ${PARSE_SRC})
add_executable(test_headerparser test/test_headerparser.cpp utils.cpp ${PARSE_SRC})

add_executable(test_utils test/utils/test_utils.cpp utils.cpp)