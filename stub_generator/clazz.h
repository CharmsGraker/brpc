#include <string>
#include <map>

#ifndef STUB_GENERATOR_CLAZZ_H
#define STUB_GENERATOR_CLAZZ_H

int PUBLIC_ACCESS = 0;
int PRIVATE_ACCESS = 1;
int PROTECTED_ACCESS = 2;

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
    void addMember(const std::string &type,
                   const std::string &classname,
                   const std::string &object_name,
                   int access = PUBLIC_ACCESS,
                   const std::string &template_arg = "") {
        members[object_name] = {
                {"template_arg", template_arg},
                {"type",         type},
                {"classname",    classname},
                {"name",         object_name},
                {"access",       std::to_string(access)}
        };
    }
};

#endif //STUB_GENERATOR_CLAZZ_H
