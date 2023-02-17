//
// Created by nekonoyume on 2023/2/16.
//

#ifndef STUB_GENERATOR_T_H
#define STUB_GENERATOR_T_H
template<class R1, class testClass2>
class testClass_proxy {
public:
    int m1()  {return impl->m1();};
    template<class L, class R>
    int m2(L ll, double & b1, int c2)  {return impl->m2();};
    int m4()  {return impl->m4();};
    double m5(int a, int b)  {return impl->m5();};
    double m6()  {return impl->m6();};
    void operator()()  {return impl->operator()();};
    void operator<<(int a)  {return impl->operator<<();};
    testClass_proxy()  {impl = new testClass();};
private:
     template<R1, testClass2>
        testClass * impl;
};
#endif //STUB_GENERATOR_T_H
