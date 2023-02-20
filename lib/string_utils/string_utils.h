//
// Created by nekonoyume on 2023/2/19.
//

#ifndef STUB_GENERATOR_STRING_UTILS_H
#define STUB_GENERATOR_STRING_UTILS_H

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <cassert>

static std::vector<std::string> split(const std::string &s, char sep) {
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

static std::vector<std::string> parse_type_param(const std::string &s) {
    // deal with special params, like & &&, and no space required if then
    std::smatch sm;
    std::regex re("(\\w+)(.*?)(\\w+)");
    std::string::const_iterator beg = s.cbegin(), end = s.cend();
    std::regex_search(beg, end, sm, re);
    assert(!sm.empty());
    int smlen = sm.size();
    return {sm[1].str(), sm[2].str(), sm[3].str()};
}

static bool beginWith(const std::string::const_iterator &begin,
                      const std::string::const_iterator &end,
                      const std::string &p) {
    auto len = end - begin;
    auto plen = p.length();
    if (len < plen)
        return false;
    int j = 0;
    for (auto iter = begin; iter != end && j < plen; ++iter, ++j) {
        if (p[j] == *iter)
            continue;
        else
            return false;
    }
    return true;
}

static std::string trim(std::string &s) {
    int slen = s.length();
    int i = 0, j = slen - 1;
    for (; i < slen; ++i) {
        if (s[i] == ' ' || s[i] == '\n')
            continue;
        else
            break;
    }
    for (; j >= i; --j) {
        if (s[j] == ' ' || s[j] == '\n')
            continue;
        else
            break;
    }
    if (i <= j) {
        return s.substr(i, j - i + 1);
    }
    return {};
}

#endif //STUB_GENERATOR_STRING_UTILS_H
