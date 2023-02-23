#include "code_analyser/headerparser.h"
#include "server_startup/ServerStartup.h"
#include "utils.h"

std::string rpc_config::project_root = get_pdir(__FILE__) + "/../..";
auto header_dir = join(rpc_config::project_root, "auto-generated/rpc_ns");
std::string stub_config::main_header_path = join(rpc_config::project_root, "auto-generated/rpc_header.h");
std::string stub_config::buttonrpc_header_path = join(rpc_config::project_root, "buttonrpc.hpp");
std::string impl_path = join(rpc_config::project_root, "auto-generated", "impl.cpp");

std::string stub_config::class_src_main_header_path = join(rpc_config::project_root,
                                                           "auto-generated/class_src_main_header.h");

int main() {
    using rpc_config::project_root;
    ServerStartup *rpc_server_startup = new ServerStartup(project_root);

    auto pdir = get_pdir(__FILE__);
    Header header(pdir + "/../testClass.h");
    {
        HeaderParser parser(header);
        parser.setExcludeClass(false);
        parser.collectAll();
    }

    rpc_server_startup->addSourcesOf(header);
    int n_cls = header.numOfClass();
    if(n_cls) {
        std::cout << "n_class=" << "\n";
        for(auto & clazz : header.clazz_objects)
            rpc_server_startup->addSourcesOf(clazz);
    }

    delete rpc_server_startup;
}