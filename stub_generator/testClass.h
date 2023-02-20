#ifndef STUB_GENERATOR_TESTCLASS_H
#define STUB_GENERATOR_TESTCLASS_H


struct base {
    base(int b1) {

    };
};


template<class R1, class testClass2>
class testClass : public base {
public:
    testClass() {}

    // @RPC_NOT_INJECT
    static int m1() {
    }
    void operator<<(int a) {

    }
    // @RPC_INJECT
    void operator()();


    template<class L, class R>
    int m2(L ll, double &b1, int c2){

    }

    void m3(double &b1) {

    };

    double m4(double &b1) {

    };
private:
    double m5() {

    }
    int main() {

    }

public:
    int m6(){

    }
};

//class ggg : public testClass {
//
//};

#endif //STUB_GENERATOR_TESTCLASS_H
