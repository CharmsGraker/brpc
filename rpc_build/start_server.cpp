int main() {
    buttonrpc server;
    server.as_server(5555);

    // @bind_area
server.bind("testClass_proxy::m2", m2);
server.bind("testClass_proxy::m3", m3);
server.bind("testClass_proxy::m4", m4);
server.bind("testClass_proxy::m6", m6);
server.bind("testClass_proxy::operator()", operator());
server.bind("testClass_proxy::operator<<", operator<<);
server.bind("testClass_proxy::testClass_proxy", testClass_proxy);
    std::cout << "run rpc server on: " << 5555 << std::endl;
    server.run();

    return 0;
}
