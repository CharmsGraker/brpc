#include <string>
#include <map>
#include "../lib/string_utils/string_utils.h"

#ifndef STUB_GENERATOR_CLAZZ_H
#define STUB_GENERATOR_CLAZZ_H

class Method;

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

struct Clazz {
    std::string classpath;
    std::string template_arg;
    std::string extend_modifier;
    std::string type;
    std::string classname;
    Method * deconstruct = nullptr;
    int n_method = 0;
    std::map<std::string, Method> methods;
    std::map<std::string, std::map<std::string, std::string>> members;
    bool done;

    Clazz() : done(false) {};

    void addDeconstruct(const std::string & body);
    std::string nameWithTemplateParams() {
        if (template_arg.empty())
            return classname;
        return classname + "<" + templateParams() + ">";
    }

    std::string templateParams() {
        int len = template_arg.length();
        size_t t_i = template_arg.find('<'), t_j = len;
        assert(t_i != template_arg.npos);

        std::string ret;
        int balance = 0;
        for (auto i = t_i; i < len; ++i) {
            if (template_arg[i] == '<')
                ++balance;
            if (template_arg[i] == '>')
                --balance;
            if (balance == 0 && i != t_i) {
                t_j = i;
                break;
            }
        }
        const std::string tp = template_arg.substr(t_i + 1, t_j - t_i - 1);
        auto t_params = split(tp, ',');
        size_t t_l = t_params.size();
        for (int i = 0; i < t_l; ++i) {
            auto &&kv = parse_type_param(t_params[i]);
            ret += kv[2];
            if (i != t_l - 1)
                ret += ',';
        }
        return ret;
    }

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

    ~Clazz() {
        if(deconstruct)
            delete deconstruct;
    }
    void
    addMethod(const std::string &name,
              const std::string &body,
              ACCESS::var access = ACCESS::PUBLIC,
              const std::string &params = "",
              const std::string &template_params = "",
              const std::string &modifier = "",
              const std::string &return_type = "");
};

#endif //STUB_GENERATOR_CLAZZ_H
