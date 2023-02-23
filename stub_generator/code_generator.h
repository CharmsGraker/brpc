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
#include "utils.h"
#include "code_analyser/parse_object/method.h"

class ClassProxy;

namespace rpc_config {
    extern std::string project_root;
}

int INLINE = 0, DECLARE = 1, DEFINITION = 2;


std::string renderMember(std::string type,
                         std::string classaname,
                         std::string object_name,
                         std::string template_params,
                         REFERENCE_TYPE::var ref_type,
                         int add_type) {
    size_t n = 0;
    char buffer[1024];
    memset(buffer, 0, sizeof buffer);
    n += sprintf(buffer, "%s %s",
                 type.c_str(), classaname.c_str());

    if (!template_params.empty())
        n += sprintf(buffer + n, "<%s>",
                     template_params.c_str());
    if (ref_type != REFERENCE_TYPE::DEFAULT) {
        n += sprintf(buffer + n, "%s", ref_type.toString().c_str());
    }
    sprintf(buffer + n, " %s;", object_name.c_str());
    return {buffer};
}

// the Clazz header will be included by other headers
std::string renderMethod(Clazz clazz, Method method, int type) {
    std::string return_type = "void", name = method["name"], params, extend_str, body;
    char buffer[2048];

    size_t n = 0;
    memset(buffer, 0, sizeof buffer);
    printf("name=*%s*\n", name.c_str());
    printf("classname=*%s*\n", clazz.classname.c_str());
    if (!method["return_type"].empty()) {
        return_type = method["return_type"];
        if (method.count("modifier")) {
            return_type = method["modifier"] + " " + return_type;
        }
    }
    if (clazz.classname == name || name == "~" + clazz.classname)
        return_type.clear();
    if (!method["template_arg"].empty())
        n += sprintf(buffer, "template<%s>\n", method["template_arg"].c_str());
    if (!method["params"].empty())
        params = method["params"];
    if (!method["body"].empty())
        body = method["body"];

    if (type == INLINE) {
        if (!method["extend_constructor"].empty())
            extend_str = " : " + method["extend_constructor"];

        sprintf(buffer + n, "%s %s(%s) %s {%s};\n",
                return_type.c_str(),
                name.c_str(),
                params.c_str(),
                extend_str.c_str(),
                body.c_str());
    } else if (type == DEFINITION) {
        sprintf(buffer + n, "%s %s::%s(%s) {\n%s\n}\n",
                return_type.c_str(),
                clazz.classname.c_str(),
                name.c_str(),
                params.c_str(),
                body.c_str());
    } else if (type == DECLARE) {
        sprintf(buffer + n, "%s %s(%s);\n",
                return_type.c_str(),
                name.c_str(),
                params.c_str());
    } else {
        abort();
    }
    return {buffer};
}

struct CodeGenerator {
private:
    void generateIncludes(std::string out) {
        for (auto &f: includes) {
            append(out, format("#include\"%s\"\n", f.c_str()));
        }
    }

    void generateForwards(std::string out) {
        for (auto &f_declare: forwards) {
            append(out, f_declare + "\n");
        }
    }

private:
    std::vector<std::string> includes;
    std::vector<std::string> forwards;
    std::string m_header_out_dir, m_header_out_path;
    std::string m_def_out_path;
public:

    CodeGenerator() = default;

    void addForward(const std::string &forward_declare) {
        forwards.push_back(forward_declare);
    }

    void addInclude(const std::string &headername) {
        includes.push_back(headername);
    }

    void setOutputDir(const std::string &dir = "") {
        m_header_out_dir = dir;
    }

    void setOutputPath(std::string path) {
        m_header_out_path = path;
    }

    void generateDefinition(std::string def_path, Clazz &clazz, std::ios::openmode mode = std::ios::out) {
        std::fstream f(def_path, mode);
        if (getExt(def_path) == ".h") {
            append(f, "class " + clazz.classname + "{\npublic:\n");
            for (auto &[name, member]: clazz.members) {
                auto d = renderMember(member["type"],
                                      member["classname"],
                                      member["name"],
                                      member["template_params"],
                                      stoi(member["reference_type"]),
                                      INLINE);
                append(f, d);
            }
            for (auto &[id, m]: clazz.methods) {
                auto d = renderMethod(clazz, m, INLINE);
                append(f, d);
            }
            append(f, "};\n");
        } else {
            append(f, format("#include\"%s/buttonrpc.hpp\"\n", rpc_config::project_root.c_str()));

            for (auto &[id, m]: clazz.methods) {
                auto d = renderMethod(clazz, m, DEFINITION);
                append(f, d);
            }
        }
        f.close();
    }


    void render(std::ios::openmode mode) {
        assert(! m_header_out_path.empty());
        generateHeader(m_header_out_path, nullptr, mode);
    }

    std::string getHeaderOutPath(Clazz &clazz) {
        if (m_header_out_dir.empty()) {
            auto cpth = clazz.getPath();
            auto pdir = get_pdir(cpth);
            return join(pdir, clazz.classname + ".h");
        } else
            return join(m_header_out_dir, clazz.classname + ".h");
    }

    void renderOf(Clazz &clazz,
                  std::ios::openmode mode,
                  std::ios::openmode d_mode = std::ios::out) {
        assert(!m_header_out_dir.empty());
        if (m_header_out_path.empty())
            m_header_out_path = getHeaderOutPath(clazz);

        if (!m_def_out_path.empty()) {
            generateDefinition(m_def_out_path, clazz, d_mode);
            generateHeader(m_header_out_path, &clazz, mode);
        } else {
            // generate definition in header
            d_mode = mode;
            generateDefinition(m_header_out_path, clazz, d_mode);
        }
    }

    void addDeconstruct(Method *de) {
    };

    std::string getOutputDir() {
        return m_header_out_dir;
    };

    void setDefinitionPath(const std::string &def_path);

private:
    void generateHeader(std::string header_path, Clazz *clazz = nullptr, std::ios::openmode mode = std::ios::out) {
        assert(getExt(header_path) == ".h");
        std::fstream f(header_path, mode);
        generateIncludes(header_path);
        generateForwards(header_path);
        if (clazz) {
            append(f, "class " + clazz->classname + "{\npublic:\n");
            for (auto &[name, member]: clazz->members) {
                auto d = renderMember(member["type"],
                                      member["classname"],
                                      member["name"],
                                      member["template_params"],
                                      stoi(member["reference_type"]),
                                      DECLARE);
                append(f, d);
            }
            for (auto &[id, m]: clazz->methods) {
                auto d = renderMethod(*clazz, m, DECLARE);
                append(f, d);
            }
            append(f, "};\n");
        }
        f.close();
    }
};

void CodeGenerator::setDefinitionPath(const std::string &def_path) {
    m_def_out_path = def_path;
}

#endif //STUB_GENERATOR_CODE_GENERATOR_H
