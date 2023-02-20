#include "code_analyser/headerparser.h"
#include "server_startup/ServerStartup.h"
#include "utils.h"
auto project_root = get_pdir(__FILE__) + "/../..";

int main() {
    ServerStartup *rpc_server_startup = new ServerStartup(project_root);

    auto pdir = get_pdir(__FILE__);
    HeaderParser parser(pdir + "/../testClass.h");
    parser.collectAllMethods();
    rpc_server_startup->addMethodsFromParser(&parser);
}