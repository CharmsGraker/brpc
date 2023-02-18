#include <string>
#include <map>

#ifndef STUB_GENERATOR_CLAZZ_H
#define STUB_GENERATOR_CLAZZ_H

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
    } PUBLIC(0), PRIVATE(1), PROTECTED(2);

    std::string var::toString() {
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
    } DEFAULT(0), POINTER(1), REFERENCE(2), RIGHT_REFERENCE(3);

    std::string var::toString() {
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
};

struct Clazz {
    std::string classpath;
    std::string template_arg;
    std::string extend_modifer;
    std::string type;
    std::string classname;
    int n_method = 0;
    std::map<std::string, std::map<std::string, std::string>> methods, members;
    bool done;

    Clazz() : done(false) {};

    void addMember(const std::string &object_name,
                   const std::string &classname,
                   const std::string &template_params,
                   REFERENCE_TYPE::var reference_type = REFERENCE_TYPE::DEFAULT,
                   ACCESS::var access = ACCESS::PUBLIC,
                   const std::string &type = "class") {
        members[object_name] = {
                {"template_params", template_params},
                {"type",            type},
                {"classname",       classname},
                {"name",            object_name},
                {"reference_type",  std::to_string(reference_type.v)},
                {"access",          std::to_string(access.v)}
        };
    }
};

#endif //STUB_GENERATOR_CLAZZ_H
