#include "clazz.h"
#include "method.h"

std::string REFERENCE_TYPE::var::toString() {
    auto &o = *this;
    if (o == REFERENCE_TYPE::DEFAULT) {
        return "";
    } else if (o == REFERENCE_TYPE::POINTER) {
        return "*";
    } else if (o == REFERENCE_TYPE::REFERENCE) {
        return "&";
    } else if (o == REFERENCE_TYPE::RIGHT_REFERENCE) {
        return "&&";
    }
    return "";
}

std::string ACCESS::var::toString() {
    auto &o = *this;
    if (o == ACCESS::PUBLIC) {
        return "public";
    } else if (o == ACCESS::PRIVATE) {
        return "private";
    } else if (o == ACCESS::PROTECTED) {
        return "protected";
    } else {
        abort();
    }
}


void cleanMethodName(std::string &name) {
    assert(!name.empty());
    if (name[0] == '~') {
        name[0] = '@';
    }
}

void Clazz::addDeconstruct(const std::string &body) {
    if (!deconstruct)
        deconstruct = new Method();
    auto &m = (*deconstruct);
    m["body"] = body;
    m["name"] = "~" + classname;
}

Clazz::~Clazz() {
    if (deconstruct)
        delete deconstruct;

}

