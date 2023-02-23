#include <string>
#include <cassert>

class ClassMem {
public:
    int bar(int arg1, std::string arg2, int arg3) {
        assert(arg1 == 10);
        assert(arg2 == "buttonrpc");
        assert(arg3 == 100);
        return arg1 * arg3;
    }
};