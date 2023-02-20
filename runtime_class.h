#include "example/rpc_ns/ClassMem.h"
#include <string>

template<typename C>
std::string getClassName(C *) {
    return "";
}

template<>
std::string getClassName(char *) {
    return "char";
};

template<>
std::string getClassName(double *) {
    return "double";
};

template<>
std::string getClassName(int *) {
    return "int";
};

// the method implement code was auto-generated, do not change!
template<>
std::string getClassName(ClassMem *) {
    return "ClassMem";
};