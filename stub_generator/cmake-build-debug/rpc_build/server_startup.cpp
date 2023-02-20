int main() {
    buttonrpc server;
    server.as_server(5555);

    // @bind_area
server.bind("testClass_proxy::m2", &testClass_proxy::m2, (testClass_proxy)nullptr);

server.bind("testClass_proxy::m3", &testClass_proxy::m3, (testClass_proxy)nullptr);

server.bind("testClass_proxy::m4", &testClass_proxy::m4, (testClass_proxy)nullptr);

server.bind("testClass_proxy::m6", &testClass_proxy::m6, (testClass_proxy)nullptr);

server.bind("testClass_proxy::operator()", &testClass_proxy::operator(), (testClass_proxy)nullptr);

server.bind("testClass_proxy::operator<<", &testClass_proxy::operator<<, (testClass_proxy)nullptr);

server.bind("testClass_proxy::testClass_proxy", &testClass_proxy::testClass_proxy, (testClass_proxy)nullptr);

    std::cout << "run rpc server on: " << 5555 << std::endl;
    server.run();

    return 0;
}
