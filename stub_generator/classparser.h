#ifndef STUB_GENERATOR_PARSER_H
#define STUB_GENERATOR_PARSER_H

#include "code_generator.h"
#include "utils.h"


class ClassParser {
protected:
    std::string m_body;
    std::string m_scope;
    Clazz clazz;
public:
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
                            // not include brace
                            m_body = m_scope.substr(l + 1, iter - beg - l - 2);
//                            m_scope = m_scope.substr(beg - m_scope.cbegin(), iter - beg);
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
        while (collectMethodOf(beg, end));
        if (clazz.n_method == 0)
            printf("find empty\n");
    };

    std::vector<std::string> split(const std::string &s, char sep) {
        std::vector<std::string> arr;
        int slen = s.length();
        int j = 0;
        for (int i = 0; i < slen; ++i) {
            while (i < slen && s[i] == sep)
                ++i;
            j = i;
            while (i < slen && s[i] != sep)
                ++i;
            if (i > j)
                arr.push_back(s.substr(j, i - j));
        }
        return arr;
    }

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
        std::cout << std::string(sem_pos + m_body.cbegin(), end);
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

    void get_scope_declare(std::string::const_iterator &beg,
                           std::string::const_iterator &end) {
        std::regex re1(".*?(public|private|protected):.*");
        auto re2 = re1;
        std::smatch sm1, sm2;
        std::regex_search(beg, end, sm1, re1);
        if (!sm1.empty())
            return sm1[1].second;
        return end;
    }

    bool collectMethodOf(std::string::const_iterator &beg,
                         std::string::const_iterator &end) {
        if (beg == end)
            return false;
        auto scope_declare = get_scope_declare(beg, end);
        assert(!scope_declare.empty());

        // class的默认访问权限是private
        if (scope_declare["scope"] == "private") {
            beg = scope_declare["end"];
            return true;
        }
        auto next = scope_declare["end"];
        std::smatch sm;
        std::regex re("\\b(template<(.*?)>)?\\s*([\\w]*?)?\\s*(.*?)\\((.*?)\\):?");
        while (beg < next && std::regex_search(beg, next, sm, re)) {
            std::map<std::string, std::string> mp;
            auto &&template_arg = sm[2].str();
            if (!template_arg.empty())
                mp["template_arg"] = template_arg;
            mp["access"] = std::to_string(ACCESS::PUBLIC.v); //TODO allow construct by string
            beg = sm[3].first;
            for (auto iter = beg; iter != next; ++iter) {
                if (std::isalpha(*iter)) {
                    beg = iter;
                    break;
                }
            }
            bool flag = true;
            std::string::const_iterator body_beg = beg, iter;
            for (iter = beg; iter != next; ++iter) {
                if (*iter == '{') {
                    body_beg = iter;
                    break;
                } else if (*iter == ';') {
                    while (iter != next && (*iter == ';' || *iter == ' ' || *iter == '\n'))
                        ++iter;
                    beg = iter;
                    flag = false;
                    break;
                }
            }

            if (!flag) {
                beg = iter;
                continue;
            }

            deal_signature(mp, beg, body_beg);
            beg = deal_function_body(mp, body_beg, next);
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

    std::string build_invoke(std::string &params) {
        return "";
    }

    std::string remove_template_declare(const std::string &template_arg) {
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
        auto tp = template_arg.substr(t_i + 1, t_j - t_i - 1);
        auto t_params = split(tp, ',');
        size_t t_l = t_params.size();
        for (int i = 0; i < t_l; ++i) {
            auto v = split(t_params[i], ' ');
            assert(v.size() == 2);

            // class, int string ... or other trivial type
            auto key = v[0];
            auto value = v[1];
            std::cout << key << " : " << value << std::endl;
            ret += value;
            if (i != t_l - 1)
                ret += ',';
        }
        return ret;
    }

    ClassProxy(std::string proxy_name, const std::string &classname,
               const std::string &classpath) : m_proxy_classname(proxy_name), ClassParser(classname, classpath) {
        char buffer[1024];
        std::set<std::string> visited;
        std::string template_params{};
        if (!clazz.template_arg.empty())
            template_params = remove_template_declare(clazz.template_arg);
        clazz.addMember("impl",
                        clazz.classname,
                        template_params,
                        REFERENCE_TYPE::POINTER,
                        ACCESS::PRIVATE,
                        clazz.type);

        for (auto &[identifier, m_mp]: clazz.methods) {
            if (visited.count(identifier))
                continue;
            auto &body = m_mp["body"];
            memset(buffer, 0, sizeof buffer);
            size_t n = 0;
            auto &&invoke_str = build_invoke(m_mp["params"]);
            auto &&r = split(identifier, '^');
            auto name = r[0];
            auto params = r.size() == 1 ? "" : r[1];

            if (name == clazz.classname) {
                // constructor
                auto &&newId = proxy_name + "^" + params;
                clazz.methods[newId] = clazz.methods[identifier];
                clazz.methods.erase(identifier);
                auto &e = clazz.methods[newId];
                e[identifier] = identifier;
                e["return_type"].clear();
                e["name"] = proxy_name;
                if (!template_params.empty()) {
                    e["body"] = format("impl = new %s<%s>(%s);",
                                       name.c_str(), template_params.c_str(), invoke_str.c_str());
                } else
                    e["body"] = format("impl = new %s(%s);",
                                       name.c_str(), invoke_str.c_str());
                visited.insert(newId);
            } else {
                n += sprintf(buffer + n, "return impl->%s(%s);",
                             name.c_str(), invoke_str.c_str());
                m_mp["body"] = std::string(buffer);
                visited.insert(identifier);
            }
        }

        clazz.classname = proxy_name;
    }
};

CodeGenerator::CodeGenerator(ClassProxy *w) :
        CodeGenerator(w->clazz) {
    for (auto &[name, mp]: w->clazz.members) {
        addMember(mp);
    }
    for (auto &[identifier, mp]: w->clazz.methods) {
        addMethod(mp);
    }
}

#endif //STUB_GENERATOR_PARSER_H
