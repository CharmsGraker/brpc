#include "parse_object.h"
#include "method.h"


void
ParseObject::addMethod(const std::string &name,
                       const std::string &body,
                       ACCESS::var access,
                       const std::string &params,
                       const std::string &template_params,
                       const std::string &modifier,
                       const std::string &return_type) {
    std::string id = name + "^" + params;
    if (methods.count(id))
        return;
    ++n_method;
    methods[id] = (std::map<std::string, std::string>) {
            {"name",            name},
            {"body",            body},
            {"access",          std::to_string(access.v)},
            {"params",          params},
            {"template_params", template_params},
            {"modifier",        modifier},
            {"return_type",     return_type}
    };
}