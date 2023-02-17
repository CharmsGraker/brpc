#ifndef STUB_GENERATOR_TESTCLASS_H
#define STUB_GENERATOR_TESTCLASS_H


struct base {
    base(int  b1) {

    };
};


template<class R1, class testClass2>
class testClass : public base{
public:
    testClass() {}
    static int m1() {
    }

    void operator<<(int a){

    }
    void operator()(){

    }

    template<class L, class R>
    int m2(L ll, double & b1, int c2);

    decltype(m1()) m3(double & b1) {

    }

public:
    int m4(){}
    double m5(int a, int b);
private:
    double m6(){}
};

//class ggg : public testClass {
//
//};

#endif //STUB_GENERATOR_TESTCLASS_H
