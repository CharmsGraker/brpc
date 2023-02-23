//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_CLASSPROXY_H
#define STUB_GENERATOR_CLASSPROXY_H

#include "code_analyser/classparser.h"
#include "code_generator.h"


extern std::string buildRPCName(const Clazz &clazz, Method method);

class ClassProxy : public ClassParser {
    std::string m_proxy_classname;
    Clazz proxyClazz;

public:
    friend CodeGenerator;

    ClassProxy(Clazz & cls) : m_proxy_classname(cls.classname + "_proxy"), ClassParser(cls) {
        char buffer[1024];
        std::set<std::string> visited;
        proxyClazz = clazz;
        // the clazz member will invoke copy constructor, but it won't affect the cls
        // so we call copy constructor explicitly
        cls = clazz;
        proxyClazz.classname = m_proxy_classname;
        proxyClazz.addMember("client",
                             "buttonrpc",
                             "",
                             REFERENCE_TYPE::POINTER,
                             ACCESS::PRIVATE,
                             "class");
        auto methods = clazz.methods;
        for (auto &&iter = methods.begin(); iter != methods.end();) {
            auto &&identifier = iter->first;
            auto &&method = iter->second;
            auto &&succ = next(iter);
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
            // constructor
            if (name == clazz.classname) {
                methods.erase(identifier);
            } else {
                auto assertStat = "assert(client);\n";
                auto invokeStat = format("client->call<%s>(\"%s\"%s).val();",
                                         method["return_type"].c_str(),
                                         buildRPCName(clazz, method).c_str(),
                                         (pv_string.empty() ? "" : (", " + pv_string).c_str()));
                if (method["return_type"] != "void") {
                    invokeStat = "return " + invokeStat;
                }
                method["body"] = std::string(assertStat + invokeStat);
                visited.insert(identifier);
            }
            iter = succ;
        }
        proxyClazz.methods = methods;
        proxyClazz.addConstruct("buttonrpc * c", "client = c;");
        proxyClazz.addConstruct();

//        proxyClazz.addDeconstruct("delete impl;");
    }

    Clazz &getProxyClass() {
        return proxyClazz;
    }
};

#endif //STUB_GENERATOR_CLASSPROXY_H
