#include "/home/nekonoyume/codeDev/CPlusPlus/buttonrpc_cpp14/stub_generator/../buttonrpc.hpp"
#include "/home/nekonoyume/codeDev/CPlusPlus/buttonrpc_cpp14/stub_generator/../auto-generated/rpc_header.h"
#include<iostream>

int main() {
    buttonrpc server;
    server.as_server(5555);

    server.bind("testClass::m3", &testClass::m3, (testClass * )
    nullptr);
    server.bind("testClass::m4", &testClass::m4, (testClass * )
    nullptr);
    server.bind("testClass::m6", &testClass::m6, (testClass * )
    nullptr);
    server.bind("testClass::operator()", &testClass::operator(), (testClass * )
    nullptr);
    server.bind("testClass::operator<<", &testClass::operator<<, (testClass * )
    nullptr);
    // @bind_area

    std::cout << "run rpc server on: " << 5555 << std::endl;
    server.run();

    return 0;
}
