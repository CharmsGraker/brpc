#ifndef STUB_GENERATOR_CONSTANT_H
#define STUB_GENERATOR_CONSTANT_H

#include <string>

namespace ACCESS {
    class var {
    public:
        int v;

        var(int x = 0) : v(x) {}

        std::string toString();

        bool operator==(ACCESS::var &v2) {
            return v == v2.v;
        }

        bool operator!=(ACCESS::var &v2) {
            return v != v2.v;
        }
    } ;
    extern var PUBLIC, PRIVATE, PROTECTED;

};
namespace REFERENCE_TYPE {
    class var {
    public:
        int v;

        var(int x = 0) : v(x) {}

        std::string toString();

        bool operator==(REFERENCE_TYPE::var &v2) {
            return v == v2.v;
        }

        bool operator!=(REFERENCE_TYPE::var &v2) {
            return v != v2.v;
        }
    };
    extern var DEFAULT, POINTER, REFERENCE, RIGHT_REFERENCE;

};
#endif //STUB_GENERATOR_CONSTANT_H
