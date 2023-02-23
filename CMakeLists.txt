cmake_minimum_required(VERSION 3.16)
project(buttonrpc_cpp14)

set(CMAKE_CXX_STANDARD 14)
find_package(cppzmq)


include_directories(.)
set(RPC_CLASS_SRC stub_generator/testClass.h)
set(RPC_STUB_SRC auto-generated/runtime_class.cpp)
set(RPC_CLIENT_STUB_SRC auto-generated/impl.cpp auto-generated/rpc_header.h)

#add_executable(buttonrpc example/main_server.cpp buttonrpc.hpp
#        Serializer.hpp )
add_executable(buttonrpc rpc_build/start_server.cpp buttonrpc.hpp
        Serializer.hpp ${RPC_STUB_SRC} ${RPC_CLASS_SRC})

add_executable(client example/main_client.cpp buttonrpc.hpp
        Serializer.hpp)
add_executable(auto_client example/auto_client.cpp buttonrpc.hpp
        Serializer.hpp ${RPC_CLIENT_STUB_SRC})

target_link_libraries(buttonrpc cppzmq)
target_link_libraries(client cppzmq)
target_link_libraries(auto_client cppzmq)

