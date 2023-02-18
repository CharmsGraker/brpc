cmake_minimum_required(VERSION 3.21)
project(buttonrpc_cpp14)

set(CMAKE_CXX_STANDARD 14)
find_package(cppzmq)


include_directories(.)

add_executable(buttonrpc example/main_server.cpp buttonrpc.hpp
        Serializer.hpp)

add_executable(client example/main_client.cpp buttonrpc.hpp
        Serializer.hpp)

target_link_libraries(buttonrpc cppzmq)
target_link_libraries(client cppzmq)
