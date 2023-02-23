#include"runtime_class.h"
template<>
std::string getClassName(ClassMem *) {
	return "ClassMem";
}
template<>
std::string getClassName(testClass_proxy *) {
	return "testClass_proxy";
}
