#include "classparser.h"
#include "code_analyser/parse_object/method.h"

bool ClassParser::collectMethodInScope(std::string::const_iterator &beg,
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
            beg = deal_function_body(mp, body_beg, body_end, m_body.cbegin(), m_body.cend());
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
        clazz.methods[mp["identifier"]] = mp;
        ++clazz.n_method;
    }
    assert(clazz.n_method == clazz.methods.size());
    beg = next;
    return true;
};