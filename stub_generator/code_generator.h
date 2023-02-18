//
// Created by nekonoyume on 2023/2/16.
//

#ifndef STUB_GENERATOR_CODE_GENERATOR_H
#define STUB_GENERATOR_CODE_GENERATOR_H

#include <bits/stdc++.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "clazz.h"
#include "utils.h"


class ClassProxy;

struct CodeGenerator;

struct Field {
    ACCESS::var access;
    std::string code;
    CodeGenerator *obj = nullptr;

    Field(CodeGenerator *o) : obj(o), access() {
    };

    std::string toString() {
        if (code.empty())
            return {};
        return access.toString() + ":\n" + code;
    }

    void insertMethod(std::map<std::string, std::string> &mp);

    void insertMember(const std::string &type,
                      const std::string &template_params,
                      const std::string &classname,
                      REFERENCE_TYPE::var &ref_type,
                      const std::string &object_name) {
        size_t n = 0;
        char buffer[1024];
        memset(buffer, 0, sizeof buffer);
        n += sprintf(buffer, "%s %s",
                     type.c_str(), classname.c_str());

        if (!template_params.empty())
            n += sprintf(buffer + n, "<%s>",
                         template_params.c_str());
        if (ref_type != REFERENCE_TYPE::DEFAULT) {
            n += sprintf(buffer + n, "%s", ref_type.toString().c_str());
        }
        sprintf(buffer + n, " %s;", object_name.c_str());
        code += std::string(buffer);
    }
};

struct CodeGenerator {
    Clazz clazz;
    std::string m_out_path;
    std::vector<Field> fields;

    CodeGenerator(ClassProxy *classProxy);

    CodeGenerator(const Clazz &c) : clazz(c),
                                    fields(3, this) {
        auto cpth = clazz.classpath;
        auto pdir = get_pdir(cpth);
        m_out_path = join(pdir, "auto-generated", clazz.classname + ".h");
        if (exist_file(m_out_path))
            remove_file(m_out_path);
        create_file(m_out_path);

        fields[0].access = ACCESS::PUBLIC;
        fields[1].access = ACCESS::PRIVATE;
        fields[2].access = ACCESS::PROTECTED;
    }

    void addMember(std::map<std::string, std::string> &mp) {
        addMember(mp["name"],
                  mp["classname"],
                  mp["template_params"],
                  mp.count("access") ? stoi(mp["access"]) : 0,
                  mp.count("reference_type") ? stoi(mp["reference_type"]) : 0,
                  mp["type"]);
    }

    void addMember(const std::string &object_name,
                   const std::string &classname,
                   const std::string &template_params,
                   const ACCESS::var &access,
                   REFERENCE_TYPE::var reference_type,
                   const std::string &type) {
        std::vector<Field>::iterator field_iter;
        if (!get_scope(access, field_iter))
            return;
        field_iter->insertMember(type,
                                 template_params,
                                 classname,
                                 reference_type,
                                 object_name);
    }

    bool get_scope(ACCESS::var access, std::vector<Field>::iterator &iter) {
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
        ACCESS::var access = (mp.count("access")
                              ? stoi(mp["access"])
                              : ACCESS::PUBLIC);
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
        assert(!m_out_path.empty());
        auto str = toString();
        write(m_out_path, str);
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
    if (!mp["return_type"].empty()) {
        return_type = mp["return_type"];
        if (mp.count("modifier")) {
            return_type = mp["modifier"] + " " + return_type;
        }
    }

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

#endif //STUB_GENERATOR_CODE_GENERATOR_H
