//
// Created by nekonoyume on 2023/2/19.
//

#ifndef STUB_GENERATOR_METHOD_H
#define STUB_GENERATOR_METHOD_H

#include <string>
#include <map>
#include <utility>
#include "../../../lib/string_utils/string_utils.h"
#include "code_analyser/parse_object/constant.h"

class Method {
    typedef std::map<std::string, std::string> container_t;
    container_t mp;
public:
    std::string identifier = "";
    std::string params = "";

    Method() {};

    Method(const container_t mp_) : mp(mp_) {
        if (mp.count("identifier"))
            identifier = mp["identifier"];
    };

    Method &operator=(container_t mp_) {
        mp = std::move(mp_);
        params = mp["params"];
        if (mp.count("identifier"))
            identifier = mp["identifier"];
        return *this;
    }

    std::string &operator[](const std::string &key) {
        return mp.operator[](key);
    }

    auto count(const std::string &key) const {
        return mp.count(key);
    }

    ACCESS::var getAccess() {
        if (mp.count("access"))
            return {stoi(mp["access"])};
        else
            return ACCESS::PUBLIC;
    }

    std::string getBody() {
        return mp["body"];
    }


    std::vector<std::string> getParamsValue() {
        params = mp["params"];
        if (params.empty())
            return {};
        std::vector<std::string> rets;
        auto vec = split(params, ',');
        for (auto &entry: vec) {
            auto kv = parse_type_param(entry);
            rets.push_back(kv[2]);
        }
        return rets;
    }

    const std::string getName() const {
        return (const_cast<std::map<std::string, std::string> &>(mp))["name"];
    }

    bool hasTemplateArg() const {
        return ! (const_cast<std::map<std::string, std::string> &>(mp))["template_arg"].empty();
    }
};

#endif //STUB_GENERATOR_METHOD_H
