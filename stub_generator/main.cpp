#include<bits/stdc++.h>
#define G_CMAKE
#include "classparser.h"

int main() {
    std::string classname = "testClass";
    auto workspace = getcwd();
#ifdef G_CMAKE
    // in cmake-debug-build folder on default
    workspace = get_pdir(workspace);
#endif
    auto class_path = workspace + "/" + classname + ".h";
    // generate new class xxx_proxy
    ClassProxy *proxy = new ClassProxy(classname + "_proxy", classname, class_path);
    auto dom = new CodeGenerator(proxy);
//    addtoRPCServer(proxy);
//    std::cout << "dom string:\n" << dom->toString();
    dom->render();
    delete dom;
    delete proxy;
}