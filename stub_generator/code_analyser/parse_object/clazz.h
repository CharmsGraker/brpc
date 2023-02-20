
#include "../../../lib/string_utils/string_utils.h"
#include "parse_object.h"
#include "constant.h"

struct Clazz : public ParseObject {
    std::string template_arg;
    std::string extend_modifier;
    std::string type;
    std::string classname;
    Method * deconstruct = nullptr;
    bool done;

    Clazz() : done(false) {};

    void addDeconstruct(const std::string & body);
    std::string nameWithTemplateParams() {
        if (template_arg.empty())
            return classname;
        return classname + "<" + templateParams() + ">";
    }

    std::string templateParams() {
        auto len = template_arg.length();
        if(len <= 0)
            return {};
        size_t t_i = template_arg.find('<'), t_j = len;
        assert(t_i != template_arg.npos);

        std::string ret;
        int balance = 0;
        for (auto i = t_i; i < len; ++i) {
            if (template_arg[i] == '<')
                ++balance;
            if (template_arg[i] == '>')
                --balance;
            if (balance == 0 && i != t_i) {
                t_j = i;
                break;
            }
        }
        const std::string tp = template_arg.substr(t_i + 1, t_j - t_i - 1);
        auto t_params = split(tp, ',');
        size_t t_l = t_params.size();
        for (int i = 0; i < t_l; ++i) {
            auto &&kv = parse_type_param(t_params[i]);
            ret += kv[2];
            if (i != t_l - 1)
                ret += ',';
        }
        return ret;
    }

    ~Clazz();

};