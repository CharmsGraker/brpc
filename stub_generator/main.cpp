#include<bits/stdc++.h>
#define G_DEBUG

#include "classparser.h"

int main() {
    std::string classname = "testClass";
    auto workspace = getcwd();
#ifdef G_DEBUG
    auto j = workspace.rfind('/');
    if (j == workspace.npos) {
        printf("error when parse workspace path!\n");
        return 0;
    }
    workspace = workspace.substr(0, j);
#endif
    auto class_path = workspace + "/" + classname + ".h";
    // generate new class xxx_proxy
    ClassWrapper *wrapper = new ClassWrapper(classname + "_proxy", classname, class_path);
    auto dom = new ProxyGenerator(wrapper);
//    addtoRPCServer(wrapper);
    std::cout << "dom string:\n" << dom->toString();
    delete dom;
    delete wrapper;
}