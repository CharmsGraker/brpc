//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_PARSE_OBJECT_H
#define STUB_GENERATOR_PARSE_OBJECT_H

#include "constant.h"
#include "method.h"

struct ParseObject {
    std::string path;
public:
    int n_method = 0;
    std::map<std::string, Method> methods;
    std::map<std::string, std::map<std::string, std::string>> members;

    ParseObject(const std::string &p) : path(p) {};

    const std::string &getPath() const {
        return path;
    }
    auto &getMethods() const {
        return methods;
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

    void
    addMethod(const std::string &name,
              const std::string &body,
              ACCESS::var access = ACCESS::PUBLIC,
              const std::string &params = "",
              const std::string &template_params = "",
              const std::string &modifier = "",
              const std::string &return_type = "");
};

#endif //STUB_GENERATOR_PARSE_OBJECT_H
