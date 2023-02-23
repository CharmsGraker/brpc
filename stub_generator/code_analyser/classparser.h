#ifndef STUB_GENERATOR_PARSER_H
#define STUB_GENERATOR_PARSER_H

#include <utility>

#include "utils.h"
#include "../../lib/string_utils/string_utils.h"
#include "code_analyser/common.h"
#include "parse_object/clazz.h"

// collect a specific class in a header, the proxy_class will be generated in rpc_ns with its classname
class ClassParser {
protected:
    std::string m_body;
    std::string m_scope;
    Clazz clazz;
public:
    Clazz &getClass() {
        return clazz;
    }

private:
    void __init() {
        m_body = trim(m_body);
        deal_default_scope(clazz.type, m_body);
#ifdef G_DEBUG
        printf("m_classname=%s\n", clazz.classname.c_str());
        printf("template_arg=%s\n", clazz.template_arg.c_str());
        printf("type=%s\n", clazz.type.c_str());
        printf("body=%s\n", m_body.c_str());
        printf("----\n");
#endif
        collectAllMethod();
    }

public:
    ClassParser(Clazz &cls) : clazz(cls) {
        if (clazz.classname.empty() || clazz.getPath().empty())
            return;
        m_scope = readFile(clazz.getPath());

        if (m_scope.empty()) {
            std::cout << "empty content\n";
            return;
        }
        std::string::const_iterator beg = m_scope.cbegin(), end = m_scope.cend();
        std::string::const_iterator body_beg, body_end;
        int n_matched = 0;
        while (beg < end) {
            if (searchFirstClassIn(beg, end, body_beg, body_end, &clazz, clazz.classname)) {
                ++n_matched;
                m_body = std::string(body_beg, body_end);
            };
            beg = body_end;
        }
        assert(n_matched == 1);
        __init();
    }

    ClassParser(Clazz &cls, std::string body) : clazz(cls), m_body(std::move(body)) {
        __init();
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