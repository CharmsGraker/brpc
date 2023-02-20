//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_CLASSPROXY_H
#define STUB_GENERATOR_CLASSPROXY_H

#include "code_analyser/classparser.h"
#include "code_generator.h"

class ClassProxy : public ClassParser {
    std::string m_proxy_classname;
public:
    friend CodeGenerator;

    ClassProxy(std::string proxy_name, const std::string &classname,
               const std::string &classpath) : m_proxy_classname(proxy_name), ClassParser(classname, classpath) {
        char buffer[1024];
        std::set<std::string> visited;
        clazz.addMember("impl",
                        clazz.classname,
                        clazz.templateParams(),
                        REFERENCE_TYPE::POINTER,
                        ACCESS::PRIVATE,
                        clazz.type);

        for (auto &[identifier, method]: clazz.methods) {
            assert(!identifier.empty());
            if (visited.count(identifier))
                continue;
            auto &&body = method.getBody();
            memset(buffer, 0, sizeof buffer);
            size_t n = 0;
            auto &&pvs = method.getParamsValue();
            std::string pv_string;
            int len = pvs.size();
            for (int i = 0; i < len; ++i) {
                pv_string += pvs[i];
                if (i != len - 1)
                    pv_string += ',';
            }
            auto &&r = split(identifier, '^');
            auto name = r[0];
            auto params = r.size() == 1 ? "" : r[1];

            if (name == clazz.classname) {
                // constructor
                auto &&newId = m_proxy_classname + "^" + params;
                clazz.methods[newId] = clazz.methods[identifier];
                clazz.methods.erase(identifier);
                auto &e = clazz.methods[newId];
                e[identifier] = identifier;
                e["return_type"].clear();
                e["name"] = m_proxy_classname;
                e["body"] = format("impl = new %s(%s);",
                                   clazz.nameWithTemplateParams().c_str(), pv_string.c_str());
                e["extend_constructor"].clear();
                visited.insert(newId);
            } else {
                if (method.count("modifier") && method["modifier"] == "static") {
                    n += sprintf(buffer + n, "return %s::%s(%s);",
                                 clazz.nameWithTemplateParams().c_str(), name.c_str(), pv_string.c_str());
                } else {
                    n += sprintf(buffer + n, "return impl->%s(%s);",
                                 name.c_str(), pv_string.c_str());
                }
                method["body"] = std::string(buffer);
                visited.insert(identifier);
            }
        }

        clazz.classname = proxy_name;
        clazz.addDeconstruct("delete impl;");
    }
};

CodeGenerator::CodeGenerator(ClassProxy *w) :
        CodeGenerator(w->clazz) {
}

#endif //STUB_GENERATOR_CLASSPROXY_H
