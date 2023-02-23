#include "common.h"

#include <regex>
#include "../../lib/string_utils/string_utils.h"
#include "parse_object/clazz.h"

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

int parse_annotation(const std::string &anno) {
#ifdef G_DEBUG
    std::cout << "\n-----annotation-----\n";
        std::cout << anno;
    std::cout << "\n--------------------\n";
#endif
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
    return flag;
}

void deal_default_scope(const std::string &clazz_type, std::string &body) {
    std::string scope_modifier = (clazz_type == "struct" ? "public" : "private");
    std::smatch sm;
    std::regex_search(body.cbegin(), body.cend(), sm, scope_re);

    if (sm.empty() || sm[1].first != body.cbegin()) {
        body = scope_modifier + ":\n" + body;
    }
}

std::string::const_iterator
deal_function_body(std::map<std::string, std::string> &mp,
                   const std::string::const_iterator &beg,
                   const std::string::const_iterator &end,
                   const std::string::const_iterator &cbegin,
                   const std::string::const_iterator &cend) {
    // 需要处理未实现的函数，否则会找到末尾去了
    auto brace_begin = std::find(beg, end, '{');
    if (brace_begin == end)
        return end;
    auto body_begin = brace_begin + 1;
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

bool searchFirstClassIn(const std::string::const_iterator &cbegin,
                        const std::string::const_iterator &cend,
                        std::string::const_iterator &from,
                        std::string::const_iterator &to,
                        Clazz *clazzPtr,
                        std::string classname) {
    if (cbegin >= cend)
        return false;
    std::regex re("(//|/[\\*])*\\s*(template<(.*?)>)?\\s*(class\\b|\\bstruct\\b)\\s+?(\\w+)(.*)");
    std::string::const_iterator beg = cbegin, end = cend;
    std::smatch sm;
    std::regex_search(beg, end, sm, re);
    if (sm.empty()) {
        from = to = end;
        return false;
    }
    if(! sm[1].str().empty()) {
        // this is an annotation
        printf("[!] class %s has been annotated.\n",sm[5].str().c_str());
        from = to = sm[5].second;
        return false;
    }

    from = sm[0].first;

    std::string::const_iterator lbraceIter = end;
    for (auto iter = sm[4].second; iter != end; ++iter) {
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

    if (lbraceIter == end) {
        // a forward declaration of class, should skip it
        to = beg;
        return false;
    }

    if (!classname.empty() && classname != sm[5].str()) {
        to = sm[5].second;
        return false;
    }
    if (clazzPtr) {
        clazzPtr->template_arg = sm[2].str();
        clazzPtr->type = sm[4].str();
        clazzPtr->classname = sm[5].str();
    }

    int balance = 0;
    std::string::const_iterator iter = lbraceIter;
    while (iter != end) {
        if (*iter == '{')
            ++balance;
        else if (*iter == '}')
            --balance;
        if (balance == 0) {
            // not include brace, but \n
            // clip the class body
            from = lbraceIter + 1;
            to = iter;
            printf("[*] class %s has been successfully found.\n",sm[5].str().c_str());

            return true;
        }
        ++iter;
    }
    return false;
}