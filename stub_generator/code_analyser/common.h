//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_COMMON_H
#define STUB_GENERATOR_COMMON_H

#include <regex>

#include "../../lib/string_utils/string_utils.h"

static std::regex scope_re(".*?(public|private|protected):.*");

void
deal_signature(std::map<std::string, std::string> &mp,
               std::string::const_iterator &beg,
               const std::string::const_iterator &end);

int parse_annotation(const std::string &anno);

extern bool default_behavior;

bool search_rpc_annotation(std::string::const_iterator &begin,
                           std::string::const_iterator &end);

void deal_default_scope(const std::string &clazz_type, std::string &body);

std::string::const_iterator
deal_function_body(std::map<std::string, std::string> &mp,
                   const std::string::const_iterator &beg,
                   const std::string::const_iterator &end,
                   const std::string::const_iterator &cbegin,
                   const std::string::const_iterator &cend);
#endif //STUB_GENERATOR_COMMON_H
