#ifndef STUB_GENERATOR_PARSER_H
#define STUB_GENERATOR_PARSER_H

#include "code_generator.h"
#include "utils.h"
#include "../lib/string_utils/string_utils.h"


static std::regex scope_re(".*?(public|private|protected):.*");

class ClassParser {
protected:
    std::string m_body;
    std::string m_scope;
public:

    Clazz clazz;
public:
    void deal_default_scope(std::string &body) {
        std::string scope_modifier = clazz.type == "struct" ? "public" : "private";
        std::smatch sm;
        std::regex_search(body.cbegin(), body.cend(), sm, scope_re);
        if (sm.empty() || sm[1].first != body.cbegin()) {
            body = scope_modifier + body;
        }
    }

    ClassParser(const std::string &classname,
                const std::string &classpath) {
        if (classname.empty() || classpath.empty())
            return;
        clazz.classname = classname;
        clazz.classpath = classpath;

        try {
            m_scope = readFile(classpath);
            if (m_scope.empty()) {
                std::cout << "empty content\n";
                return;
            }

            char str[256];
            sprintf(str,
                    "\\b(template<(.*?)>)?\\s*(class\\b|\\bstruct\\b)\\s+?\\b(%s)\\b:?\\s*",
                    classname.c_str());
            std::string::const_iterator beg = m_scope.cbegin(), end = m_scope.cend();
            std::regex re((std::string(str)));
            std::smatch sm;
            int n_matched = 0;
            while (std::regex_search(beg, end, sm, re)) {
//                printf("prefix=%s\n", sm.prefix().str().c_str());
//                printf("suffix=%s\n", sm.suffix().str().c_str());
//                printf("name=%s\n", sm[4].str().c_str());
                clazz.template_arg = sm[1].str();
                clazz.type = sm[3].str();

                auto l = m_scope.find('{', sm[0].first - m_scope.cbegin());
                std::cout << "m_scope=\n" << m_scope.substr(sm[0].first - m_scope.cbegin());
                if (l != m_scope.npos) {
                    ++n_matched;
                    int balance = 1;
                    std::string::const_iterator iter = m_scope.cbegin() + l;
                    ++iter;
                    while (iter != end) {
                        if (*iter == '{')
                            ++balance;
                        else if (*iter == '}')
                            --balance;
                        ++iter;
                        if (balance == 0) {
                            // not include brace, but \n
                            m_body = m_scope.substr(l + 1, iter - beg - l - 2);
//                            m_scope = m_scope.substr(beg - m_scope.cbegin(), iter - beg);
                            m_body = trim(m_body);
                            deal_default_scope(m_body);

                            beg = iter;
                            break;
                        }
                    }
                }
                beg = sm[0].second;
            }

            if (n_matched != 1) {
                std::cout << "match fail\n";
                return;
            }
//            std::cout << "n_matched=" << n_matched << std::endl;
//            for (int i = 0; i < n_matched; ++i) {
//                printf("[%d]:\n%s\n", i, rets[i].c_str());
//            }
            assert(n_matched == 1);
            printf("m_classname=%s\n", clazz.classname.c_str());
            printf("template_arg=%s\n", clazz.template_arg.c_str());
            printf("type=%s\n", clazz.type.c_str());
            printf("body=%s\n", m_body.c_str());
            printf("----\n");
            collectAllMethod();
        } catch (std::exception &e) {
            std::cout << e.what();
        }
    };

    void collectAllMethod() {
        // 先处理省略了访问修饰符的，比如class默认为private,这个只会出现在第一段
        std::string::const_iterator beg = m_body.cbegin(), end = m_body.cend();
        while (collectMethodInScope(beg, end));
        if (clazz.n_method == 0)
            printf("find empty\n");
    };


private:
    void
    deal_signature(std::map<std::string, std::string> &mp,
                   std::string::const_iterator &beg,
                   const std::string::const_iterator &end) {

        auto &&arr = split(std::string(beg, end), ':');
        if (arr.size() == 2)
            mp["extend_constructor"] = arr[1];
        std::string s = arr[0];
        std::string::const_iterator sbeg = s.cbegin(), send = s.cend();
        std::regex re2("(virtual|static)?\\b(.*)\\((.*?)\\)");
        std::smatch sm;
        std::regex_search(sbeg, send, sm, re2);
        mp["modifier"] = sm[1].str();
        auto &&ret = split(sm[2].str(), ' ');
        if (ret.size() == 2) {
            mp["return_type"] = ret[0];
            mp["name"] = ret[1];
        } else if (ret.size() == 1)
            mp["name"] = ret[0];
        else {
            printf("**********%s\n", sm[2].str().c_str());
            for (auto &r: ret) {
                std::cout << r << std::endl;
            }
        }
        mp["params"] = sm[3].str();
        mp["identifier"] = mp["name"] + "^" + mp["params"];
    }

    std::string::const_iterator
    deal_function_body(std::map<std::string, std::string> &mp,
                       const std::string::const_iterator &beg,
                       const std::string::const_iterator &end) {
        int p;
        auto sem_pos = m_body.find(';', beg - m_body.cbegin());
//        std::cout << std::string(sem_pos + m_body.cbegin(), end);
        // 需要处理未实现的函数，否则会找到末尾去了
        if ((p = m_body.find('{', beg - m_body.cbegin())) == m_body.npos
            || p + m_body.cbegin() >= end)
            return end;
        auto body_begin = p + m_body.cbegin() + 1;
        auto body_end = end;
        auto cur = body_begin;
        int balance = 1;
        while (cur != end) {
            if (*cur == '{') {
                ++balance;
            } else if (*cur == '}')
                --balance;
            if (balance == 0) {
                body_end = cur;
                break;
            }
            ++cur;
        }
        mp["body"] = std::string(body_begin, body_end);
        return cur;
    }

    int parse_annotation(const std::string &anno) {
        std::cout << anno << std::endl;
        std::regex re("@RPC_(\\w+)\\b");
        std::smatch sm;
        std::string::const_iterator beg = anno.cbegin(), end = anno.cend();
        int flag = 0;
        while (std::regex_search(beg, end, sm, re)) {
            if (!sm.empty()) {
                if (sm[1].str() == "INJECT") {
                    flag |= 1;
                } else if (sm[1].str() == "NOT_INJECT") {

                } else {
                    // skip this method
                }
                beg = sm[1].second;
            }
        };
        return flag;
    }

    bool default_behavior = true;

    bool search_rpc_annotation(std::string::const_iterator &begin,
                               std::string::const_iterator &end) {
        std::cout << "\n-----annotation-----\n";
        std::regex re("\\s*(//\\s*(.*?)\\s*?\r*\n|/\\*\\s*(.*?)\\s*\\*/)");
        std::smatch sm;
        int flag = -1;
        while (std::regex_search(begin, end, sm, re)) {
            if (!sm.empty()) {
                std::string anno;
                if (!sm[2].str().empty()) {
                    anno = sm[2].str();
                    begin = sm[2].second;
                } else if (!sm[3].str().empty()) {
                    anno = sm[3].str();
                    begin = sm[3].second;
                } else {
                    begin = end;
                }
                if (!anno.empty())
                    flag = std::max(0, flag) | parse_annotation(anno);
            }
        }

        if (flag < 0)
            return default_behavior;
        std::cout << flag;
        return flag;
    }

    bool collectMethodInScope(std::string::const_iterator &beg,
                              std::string::const_iterator &end) {
        if (beg == end)
            return false;
        std::string::const_iterator next,
                last,
                tbegin; // template begin, equal to begin if not exist template arguments

        std::smatch sm1, sm2;

        next = end;

        // locate current scope starter
        std::regex_search(beg, end, sm1, scope_re);
        std::string scope_modifier;
        if (!sm1.empty()) {
            beg = sm1[1].second + 1; // to skip the ':'
            scope_modifier = sm1[1].str();
            // locate next scope starter if exists
            std::regex_search(sm1[1].second + 1, end, sm2, scope_re);
            if (!sm2.empty()) {
                next = sm2[1].first;
                assert(beg != next);
            }
        }

        if (scope_modifier == "private") {
            beg = next;
            return true;
        }

        // TODO: accept add all simple methods in a header
        // collectMethodInSegment();
        // should not match annotation in regex pattern
        std::smatch sm;
        std::regex re("\\b(template<(.*?)>)?\\s*([\\w[^\\s]]*?)?\\s*(.*?)\\((.*?)\\):?");
        for (last = beg;
             beg < next && std::regex_search(beg, next, sm, re);
             last = beg) {
            std::map<std::string, std::string> mp;
            auto &&template_arg = sm[2].str();

            //TODO: allow construct by string
            mp["access"] = std::to_string(ACCESS::PUBLIC.v);
            beg = sm[3].first;
            if (!template_arg.empty()) {
                mp["template_arg"] = template_arg;
                tbegin = sm[1].first;
            } else
                tbegin = beg;
            std::string::const_iterator body_end = next;

            bool only_declaration = false;
            std::string::const_iterator body_beg = beg, iter;
            for (iter = beg; iter != next; ++iter) {
                if (*iter == '{') {
                    body_beg = iter;
                    break;
                } else if (*iter == ';') {
                    body_beg = iter;
                    while (iter != next && (*iter == ';' || *iter == ' ' || *iter == '\n'))
                        ++iter;
                    body_end = iter;
                    only_declaration = true;
                    break;
                }
            }

            std::cout << "signature: " << std::string(beg, body_beg) << "\n";
            deal_signature(mp, beg, body_beg);

            // filter those methods do not want to be injected
            bool injectFlag = default_behavior;
            if (last < tbegin) {
                injectFlag = search_rpc_annotation(last, tbegin);
            }
            if (only_declaration) {
                beg = body_end;
            } else
                beg = deal_function_body(mp, body_beg, body_end);
            if(! injectFlag)
                continue;
#ifdef G_DEBUG
                printf("template_arg:%-15s\n", template_arg.c_str());
                std::cout << std::setw(15);
                std::cout << "modifier=" << mp["modifier"] << std::endl;
                std::cout << "return_type=" << mp["return_type"] << std::endl;
                std::cout << "identifier=" << mp["identifier"] << std::endl;
                std::cout << "name=" << mp["name"] << std::endl;
                std::cout << "params=" << mp["params"] << std::endl;
                std::cout << "body=" << mp["body"] << std::endl;
                printf("-----------------------------\n");
#endif
            assert(mp.count("name"));
            clazz.methods[mp["identifier"]] = mp;
            ++clazz.n_method;
        }
        assert(clazz.n_method == clazz.methods.size());
        beg = next;
        return true;
    };
};

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


#endif //STUB_GENERATOR_PARSER_H
