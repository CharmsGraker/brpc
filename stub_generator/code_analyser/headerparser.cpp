#include "headerparser.h"
#include "code_analyser/classparser.h"
#include "code_analyser/parse_object/method.h"

void HeaderParser::collectMethodsBetween(const std::string::const_iterator &cbegin,
                                         const std::string::const_iterator &cend) {
    assert(m_header);

    std::smatch sm;
    std::regex re("\\b(template<(.*?)>)?\\s*([\\w[^\\s]]*?)?\\s*(.*?)\\((.*?)\\):?");
    std::string::const_iterator beg = cbegin, end = cend;
    std::string::const_iterator last, tbegin;
    for (last = beg;
         beg < end && std::regex_search(beg, end, sm, re);
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
        std::string::const_iterator body_end = end;

        bool only_declaration = false;
        std::string::const_iterator body_beg = beg, iter;
        for (iter = beg; iter != end; ++iter) {
            if (*iter == '{') {
                body_beg = iter;
                break;
            } else if (*iter == ';') {
                body_beg = iter;
                while (iter != end && (*iter == ';' || *iter == ' ' || *iter == '\n'))
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
            beg = deal_function_body(mp, body_beg, body_end, cbegin, cend);
        if (!injectFlag)
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
        m_header->methods[mp["identifier"]] = mp;
        ++m_header->n_method;
    }
    assert(m_header->n_method == m_header->methods.size());
}

void HeaderParser::collectAll() {
    assert(m_header);
    std::string::const_iterator cbegin = m_body.cbegin(), cend = m_body.cend();
    std::string::const_iterator beg = cbegin, end = cend;
    std::string::const_iterator class_begin, class_end;
    while (beg < end) {
        if (!m_exclude_class) {
            Clazz clazz;
            if (searchFirstClassIn(beg, end, class_begin, class_end, &clazz)) {
                ClassParser parser(clazz, std::string(class_begin, class_end));
                m_header->clazz_objects.emplace_back(std::move(clazz));
            }
        } else
            searchFirstClassIn(beg, end, class_begin, class_end);
        if (beg < class_begin)
            collectMethodsBetween(beg, class_begin);
        beg = class_end;
    };
}