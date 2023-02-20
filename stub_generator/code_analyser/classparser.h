#ifndef STUB_GENERATOR_PARSER_H
#define STUB_GENERATOR_PARSER_H

#include "utils.h"
#include "../../lib/string_utils/string_utils.h"
#include "code_analyser/common.h"
#include "parse_object/clazz.h"

class ClassParser {
protected:
    std::string m_body;
    std::string m_scope;
public:
    Clazz clazz;
public:
    ClassParser(const std::string &classname,
                const std::string &classpath) {
        if (classname.empty() || classpath.empty())
            return;
        clazz.classname = classname;
        clazz.path = classpath;
        m_scope = readFile(classpath);


        if (m_scope.empty()) {
            std::cout << "empty content\n";
            return;
        }

        char str[256];
        sprintf(str,
                "\\b(template<(.*?)>)?\\s*(class\\b|\\bstruct\\b)\\s+?(%s)(.*)",
                classname.c_str());
        std::string::const_iterator beg = m_scope.cbegin(), end = m_scope.cend();
        std::regex re((std::string(str)));
        std::smatch sm;
        int n_matched = 0;
        while (std::regex_search(beg, end, sm, re)) {
//            int i = 0;
//            for (auto &s: sm) {
//                std::cout << i++ << " " << s << std::endl;
//            }

            clazz.template_arg = sm[1].str();
            clazz.type = sm[3].str();
            std::string::const_iterator lbraceIter = end;
            for (auto iter = sm[3].second; iter != end; ++iter) {
                if (*iter == ';') {
                    if (lbraceIter == end) {
                        // this a forward declaration of class
                        beg = iter;
                        break;
                    }
                } else if (*iter == '{') {
                    lbraceIter = iter;
                    break;
                }
            }
            // \s+?\b(%s)\b:?\s*
            if (lbraceIter == end) { // a forward declaration of class
                beg = sm[4].second;
                continue;
            }

            ++n_matched;
            int balance = 0;
            std::string::const_iterator iter = lbraceIter;
            while (iter != end) {
                if (*iter == '{')
                    ++balance;
                else if (*iter == '}')
                    --balance;
                ++iter;
                if (balance == 0) {
                    // not include brace, but \n
                    m_body = std::string(lbraceIter + 1, iter - 1);
//                            m_scope = m_scope.substr(beg - m_scope.cbegin(), iter - beg);
                    m_body = trim(m_body);
                    deal_default_scope(clazz.type, m_body);
                    beg = iter;
                    break;
                }
            }

            // if(beg < class_beg) {
            //      collectSimpleMethod();
            //      beg = class_beg;
            // }
            beg = sm[0].second;
        }

        assert(n_matched == 1);
        printf("m_classname=%s\n", clazz.classname.c_str());
        printf("template_arg=%s\n", clazz.template_arg.c_str());
        printf("type=%s\n", clazz.type.c_str());
        printf("body=%s\n", m_body.c_str());
        printf("----\n");
        collectAllMethod();
    };

    void collectAllMethod() {
        // 先处理省略了访问修饰符的，比如class默认为private,这个只会出现在第一段
        std::string::const_iterator beg = m_body.cbegin(), end = m_body.cend();
        while (collectMethodInScope(beg, end));
        if (clazz.n_method == 0)
            printf("find empty\n");
    };

    auto &getMethods() const {
        return clazz.methods;
    }

private:
    bool collectMethodInScope(std::string::const_iterator &beg,
                              std::string::const_iterator &end);
};

#endif //STUB_GENERATOR_PARSER_H