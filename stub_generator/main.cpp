#include<bits/stdc++.h>

#define G_CMAKE

#include "classparser.h"
#include "server_startup/ServerStartup.h"

ServerStartup *rpc_server_startup = nullptr;
auto project_root = get_pdir(__FILE__) + "/..";


void convertRPCClass(const std::string &classname, const std::string &class_path) {
    // generate new class xxx_proxy
    ClassProxy *proxy = new ClassProxy(classname + "_proxy", classname, class_path);
    auto dom = new CodeGenerator(proxy);
    dom->setOutputDir(project_root);
    rpc_server_startup->addClass(proxy);
    dom->render();
    rpc_server_startup->buildAll();

    delete dom;
    delete proxy;
}

int main() {
    rpc_server_startup = new ServerStartup(project_root);
    std::string classname = "testClass";
    auto class_path = join(project_root, "stub_generator", classname + ".h");
    convertRPCClass(classname, class_path);
    delete rpc_server_startup;
}