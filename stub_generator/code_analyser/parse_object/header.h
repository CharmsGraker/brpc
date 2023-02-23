//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_HEADER_H
#define STUB_GENERATOR_HEADER_H

#include "parse_object.h"

class Clazz;

struct Header : public ParseObject {
    Header(const std::string &path_) : ParseObject(path_) {};
    std::vector<Clazz> clazz_objects;

    auto numOfClass() {
        return clazz_objects.size();
    }


};

#endif //STUB_GENERATOR_HEADER_H
