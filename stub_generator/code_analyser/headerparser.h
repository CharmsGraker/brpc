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

class Clazz;
// this class will register all method in one header
class HeaderParser {
    std::string m_body;
    Header header;
public:
    HeaderParser(const std::string &header_path) {
        m_body = readFile(header_path);
        if (m_body.empty()) {
            std::cout << "empty content\n";
            return;
        }
    }

    void searchClassIn(const std::string::const_iterator &cbegin,
                       const std::string::const_iterator &cend,
                       std::string::const_iterator &from,
                       std::string::const_iterator &to,
                       Clazz *clazzPtr = nullptr);

    void collectMethodsBetween(const std::string::const_iterator &cbegin,
                               const std::string::const_iterator &cend);


    void collectAllMethods() {
        std::string::const_iterator cbegin = m_body.cbegin(), cend = m_body.cend();
        std::string::const_iterator beg = cbegin, end = cend;
        std::string::const_iterator class_begin, class_end;

        while (beg < end) {
            searchClassIn(beg, end, class_begin, class_end);
            if (beg < class_begin)
                collectMethodsBetween(beg, class_begin);
            beg = class_end;
        };
    }

    void printAllMethods();

    auto &getMethods() const {
        return header.methods;
    }


    auto &&getPath() {
        return header.getPath();
    }
};

#endif //STUB_GENERATOR_HEADERPARSER_H
