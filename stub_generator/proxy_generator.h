//
// Created by nekonoyume on 2023/2/16.
//

#ifndef STUB_GENERATOR_PROXY_GENERATOR_H
#define STUB_GENERATOR_PROXY_GENERATOR_H

#include <bits/stdc++.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "clazz.h"


class ClassWrapper;

struct ProxyGenerator;

struct Field {
    int access = PUBLIC_ACCESS;
    std::string scope;
    std::string code;
    ProxyGenerator *obj = nullptr;

    Field(ProxyGenerator *o) : obj(o) {
    };

    void insertMethod(std::map<std::string, std::string> &mp);

    void insertMember(const std::string &type,
                      const std::string &classname,
                      const std::string &object_name) {
        char buffer[256];
        sprintf(buffer, "%s %s %s;", type.c_str(), classname.c_str(), object_name.c_str());
        code += std::string(buffer);
    }

    std::string toString() {
        if (access == PUBLIC_ACCESS) {
            scope = "public:\n";
        } else if (access == PRIVATE_ACCESS) {
            scope = "private:\n";
        } else if (access == PROTECTED_ACCESS) {
            scope = "protected:\n";
        } else {
            exit(1);
        }
        if (!code.empty()) {
            return scope + code;
        }
        return "";
    };
};

struct ProxyGenerator {
    Clazz clazz;
    std::string m_out_path;
    std::vector<Field> fields;

    ProxyGenerator(ClassWrapper *classWrapper);

    ProxyGenerator(const Clazz &c) : clazz(c),
                                     fields(3, this) {
//        create_file(m_out_path);
//        if (exist_file(m_out_path))
//            remove_file(m_out_path);
        fields[0].access = PUBLIC_ACCESS;
        fields[1].access = PRIVATE_ACCESS;
        fields[2].access = PROTECTED_ACCESS;
    }

    void addMember(std::map<std::string, std::string> &mp) {
        addMember(mp["type"], mp["classname"], mp["name"], stoi(mp["access"]));
    }

    void addMember(const std::string &type,
                   const std::string &classname,
                   const std::string &object_name,
                   int access,
                   const std::string &initializer = "") {
        std::vector<Field>::iterator field_iter;
        if (!get_scope(access, field_iter))
            return;
        field_iter->insertMember(type, classname, object_name);
    }

    bool get_scope(int access, std::vector<Field>::iterator &iter) {
        for (auto &&f = fields.begin(); f != fields.end(); ++f) {
            if (f->access == access) {
                iter = f;
                return true;
            }
        }
        printf("wrong access type of class object!\n");
        return false;
    }

    void addMethod(std::map<std::string, std::string> &mp) {
        std::vector<Field>::iterator field_iter;
        int access = (mp.count("access")
                      ? stoi(const_cast<std::map<std::string, std::string> &>(mp)["access"])
                      : PUBLIC_ACCESS);
        if (!get_scope(access, field_iter))
            return;
        field_iter->insertMethod(mp);
    };

    std::string toString() {
        std::string header = "#include\"" + clazz.classpath + "\"\n\n";
        std::string stream, fieldStr, template_arg;
        if (!clazz.template_arg.empty()) {
            template_arg = clazz.template_arg;
        }
        for (auto &f: fields) {
            fieldStr += f.toString();
        }
        stream = header + template_arg + clazz.type + " " + clazz.classname + " {\n" + fieldStr + "};";
        return stream;
    }

    void render() {
        if (m_out_path.empty())
            return;
        auto str = toString();
        std::ofstream f(m_out_path);
        f << str;
        f.close();
    }
};

void Field::insertMethod(std::map<std::string, std::string> &mp) {
//    assert(mp.count("name"));
    assert(obj);
    std::string return_type = "void", name = mp["name"], params, extend_str, body;

    char buffer[2048];
    size_t n = 0;
    memset(buffer, 0, sizeof buffer);
    auto &&clazz = obj->clazz;
    printf("name=*%s*\n", name.c_str());
    printf("classname=*%s*\n", clazz.classname.c_str());
    if (clazz.classname == name)
        return_type.clear();
    if (!mp["return_type"].empty())
        return_type = mp["return_type"];

    if (!mp["extend_constructor"].empty())
        extend_str = " : " + mp["extend_constructor"];
    if (!mp["template_arg"].empty())
        n = sprintf(buffer, "template<%s>\n", mp["template_arg"].c_str());
    if (!mp["params"].empty())
        params = mp["params"];
    if (!mp["body"].empty())
        body = mp["body"];

    sprintf(buffer + n, "%s %s(%s) %s {%s};\n",
            return_type.c_str(),
            name.c_str(),
            params.c_str(),
            extend_str.c_str(),
            body.c_str());
    std::string func_str = std::string(buffer);
    code += func_str;
}

#endif //STUB_GENERATOR_PROXY_GENERATOR_H
