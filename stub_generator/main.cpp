#include<bits/stdc++.h>

#define G_CMAKE
#define G_DEBUG

#include "server_startup/ServerStartup.h"

ServerStartup *rpc_server_startup = nullptr;
std::string rpc_config::project_root = get_pdir(__FILE__) + "/..";
auto header_dir = join(rpc_config::project_root, "auto-generated/rpc_ns");
std::string stub_config::main_header_path = join(rpc_config::project_root, "auto-generated/rpc_header.h");
std::string stub_config::buttonrpc_header_path = join(rpc_config::project_root, "buttonrpc.hpp");
std::string impl_path = join(rpc_config::project_root, "auto-generated", "impl.cpp");

std::string stub_config::class_src_main_header_path = join(rpc_config::project_root,
                                                           "auto-generated/class_src_main_header.h");

void convertRPCClass(const std::string &classname, const std::string &class_path) {
    using namespace stub_config;
    // generate new class xxx_proxy
    Clazz rpcClazz(classname, class_path);
    ClassProxy proxy(rpcClazz);

    CodeGenerator gen;
    gen.setOutputDir(header_dir);
    gen.setDefinitionPath(impl_path);
    gen.addForward("class buttonrpc;");
    gen.renderOf(proxy.getProxyClass(), std::ios::out, std::ios::app);

    rpc_server_startup->addSourcesOf(rpcClazz);
}

std::vector<std::pair<std::string, std::string>>
parse_argument(int argc, char *argv[]) {
//    if (argc == 1) {
//        printf("wrong param when execute %s\n", argv[0]);
//        abort();
//    } else if(argc == 2) {
//
//    }
    std::string classname = "testClass";
    auto class_path = join(rpc_config::project_root, "stub_generator", classname + ".h");
    return {{classname, class_path}};
}

int main(int argc, char *argv[]) {
    auto classes = parse_argument(argc, argv);
    rpc_server_startup = new ServerStartup(rpc_config::project_root);

    for (auto &[classname, class_path]: classes) {
        convertRPCClass(classname, class_path);
    }

    delete rpc_server_startup;
    auto dir = get_pdir(__FILE__);
    system(format("python %s/scripts/build.py", dir.c_str()).c_str());
}