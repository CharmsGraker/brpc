#include "clazz.h"
#include "method.h"

namespace ACCESS {
    var PUBLIC(0), PRIVATE(1), PROTECTED(2);
};
namespace REFERENCE_TYPE {
    var DEFAULT(0), POINTER(1), REFERENCE(2), RIGHT_REFERENCE(3);
};

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

void
Clazz::addMethod(const std::string &name,
                 const std::string &body,
                 ACCESS::var access,
                 const std::string &params,
                 const std::string &template_params,
                 const std::string &modifier,
                 const std::string &return_type) {
    std::string id = name + "^" + params;
    if (methods.count(id))
        return;
    ++n_method;
    methods[id] = (std::map<std::string, std::string>) {
            {"name",            name},
            {"body",            body},
            {"access",          std::to_string(access.v)},
            {"params",          params},
            {"template_params", template_params},
            {"modifier",        modifier},
            {"return_type",     return_type}
    };
}

void Clazz::addDeconstruct(const std::string & body) {
    if (!deconstruct)
        deconstruct = new Method();
    auto & m = (*deconstruct);
    m["body"] = body;
    m["name"] = "~" + classname;
}

