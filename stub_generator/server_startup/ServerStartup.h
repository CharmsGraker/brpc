//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_SERVERSTARTUP_H
#define STUB_GENERATOR_SERVERSTARTUP_H

#include "classparser.h"

struct Statement {
    Statement() = default;

    std::string left, right;
    std::string params;
    std::string op;

    std::string toString() {
        if (!left.empty()) {
            if (op == "call") {
                return left + "(" + ")";
            }
            return left + op + right + ";";

        } else {
            assert(op.empty());
            return {};
        }
    }
};

struct Block {
    typedef std::string stat_t;
    std::vector<stat_t> statements;

    void insert(const stat_t &stat) {
        statements.push_back(stat);
    };

    std::string render() {
        std::string rets;
        int n = statements.size();
        for (int i = 0; i < n; ++i) {
            rets += statements[i];
            if (i != n - 1)
                rets += '\n';
        }
        return rets;
    }
};

class ServerStartup {
    std::string template_location;
    std::string server_root;
    Block start_area;
    Block run_area;

public:
    ServerStartup() = delete;

    ServerStartup(const std::string &root) : server_root(root) {};

    Statement build_method_startup(const Method &m) {
        return {};
    }

public:
    void addClass(ClassProxy *proxy) {
        auto &clazz = proxy->clazz;
        for (auto &[name, method]: clazz.methods) {
            auto rpc_m_name = format("%s::%s",
                                     clazz.classname.c_str(), method["name"].c_str());
            auto stat = format("server.bind(\"%s\", &%s::%s, (%s)%s);",
                               rpc_m_name.c_str(),
                               clazz.classname.c_str(),
                               method["name"].c_str(),
                               clazz.classname.c_str(),
                               "nullptr");
//            std::cout << stat << std::endl;
            run_area.insert(stat);
        }
    };

    void buildAll() {
        assert(!server_root.empty());
        auto f = readFile(server_root + "/conf/run.template");
        std::string::const_iterator beg = f.cbegin(), end = f.cend();
        std::smatch sm;
        std::regex ra_re("// @bind_area\n");
        std::regex_search(beg, end, sm, ra_re);
        assert(!sm.empty());
        auto iter = sm[0].second;
        f.insert(iter - beg, run_area.render());
        auto statup_file_path = server_root + "/rpc_build/start_server.cpp";
        create_file(statup_file_path);
        write(statup_file_path, f);
    }
};

#endif //STUB_GENERATOR_SERVERSTARTUP_H
