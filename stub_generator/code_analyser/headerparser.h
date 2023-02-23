//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_HEADERPARSER_H
#define STUB_GENERATOR_HEADERPARSER_H

#include <string>
#include <iostream>
#include <regex>
#include "utils.h"
#include "parse_object/constant.h"
#include "parse_object/header.h"
#include "code_analyser/common.h"

class ClassParser;

// this class will register all method in one header
class HeaderParser {
    std::string m_body;
    Header *m_header = nullptr;
    bool m_exclude_class;
public:
    HeaderParser(Header &header, bool excludeClass = true) :
            m_header(&header), m_exclude_class(excludeClass) {
        m_body = readFile(header.getPath());
        if (m_body.empty()) {
            std::cout << "empty content\n";
            return;
        }
    }

    bool setExcludeClass(bool flag) {
        bool old = m_exclude_class;
        m_exclude_class = flag;
        return old;
    }

    Header *getHeader() {
        return m_header;
    }

    void collectMethodsBetween(const std::string::const_iterator &cbegin,
                               const std::string::const_iterator &cend);

    void collectAllMethods() {
        bool tmp = m_exclude_class;
        m_exclude_class = true;
        collectAll();
        m_exclude_class = tmp;
    }

    void collectAll();

    // those class with only declaration will be ignored

    ~HeaderParser() {
        // release ref
        m_header = nullptr;
    }

};

#endif //STUB_GENERATOR_HEADERPARSER_H
