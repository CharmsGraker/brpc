#include"/home/nekonoyume/codeDev/CPlusPlus/buttonrpc_cpp14/stub_generator/testClass.h"

template<class R1, class testClass2>
class testClass_proxy {
public:
    static int m1() { return impl->m1(); };

    template<class L, class R>
    int m2(L ll, double &b1, int c2) { return impl->m2(); };

    void m3(double &b1) { return impl->m3(); };

    double m4(double &b1) { return impl->m4(); };

    double m5() { return impl->m5(); };

    int m6() { return impl->m6(); };

    int main() { return impl->main(); };

    void operator<<(int a) { return impl->operator<<(); };

    testClass_proxy() { impl = new testClass<R1, testClass2>(); };
private:
    class testClass<R1, testClass2> *impl;
};