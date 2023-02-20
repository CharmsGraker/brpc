#include"/home/nekonoyume/codeDev/CPlusPlus/buttonrpc_cpp14/stub_generator/../stub_generator/testClass.h"

class testClass_proxy {
public:
template<class L, class R>
 int m2(L ll, double &b1, int c2)  {return impl->m2(ll,b1,c2);};
 void m3(double &b1)  {return impl->m3(b1);};
 double m4(double &b1)  {return impl->m4(b1);};
 int m6()  {return impl->m6();};
 void operator()()  {return impl->operator()();};
 void operator<<(int a)  {return impl->operator<<(a);};
 testClass_proxy(int b1)  {impl = new testClass(b1);};
 ~testClass_proxy()  {delete impl;};
private:
class testClass* impl;};