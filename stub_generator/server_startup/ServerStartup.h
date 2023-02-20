//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_SERVERSTARTUP_H
#define STUB_GENERATOR_SERVERSTARTUP_H

#include "classproxy.h"
#include "code_analyser/headerparser.h"

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


public:
    void addClass(ClassParser *parser) {
        return addMethodsFromParser(parser);
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

    void addRPCMethod(std::string &rpcName, Method &method, const std::string &classname = "") {
        std::string stat;
        if (classname.empty()) {
            stat = format("server.bind(\"%s\", %s);",
                          rpcName.c_str(),
                          method["name"].c_str());
        } else {
            stat = format("server.bind(\"%s\", &%s::%s, (%s)%s);",
                          rpcName.c_str(),
                          classname.c_str(),
                          method["name"].c_str(),
                          classname.c_str(),
                          "nullptr");
        }

//            std::cout << stat << std::endl;
        run_area.insert(stat);
    }

    void addMethodsFromParser(ClassParser *parser);

    void addMethodsFromParser(HeaderParser *parser);

    std::string buildRPCName(ClassParser *parser, Method &method);

    std::string buildRPCName(HeaderParser *parser, Method &method);

};


void ServerStartup::addMethodsFromParser(ClassParser *parser) {
    const std::map<std::string, Method> &methods = parser->getMethods();
    auto &clazz = parser->clazz;

    for (auto &[id, m]: methods) {
        auto m_rpcname = buildRPCName(parser, const_cast<Method &> (m));
        addRPCMethod(m_rpcname, const_cast<Method &> (m));
    }
}

void ServerStartup::addMethodsFromParser(HeaderParser *parser) {
    const std::map<std::string, Method> &methods = parser->getMethods();
    for (auto &[id, m]: methods) {
        auto m_rpcname = buildRPCName(parser, const_cast<Method &> (m));
        addRPCMethod(m_rpcname, const_cast<Method &>(m));
    }
}

std::string getRelativeFromRoot(const std::string & root, const std::string & file_path) {
    return {};
}

std::string ServerStartup::buildRPCName(HeaderParser *parser, Method &method) {
    return format("%s::%s", getRelativeFromRoot(server_root, parser->getPath()).c_str(), method["name"].c_str());
}

std::string ServerStartup::buildRPCName(ClassParser *parser, Method &method) {
    return format("%s::%s", parser->clazz.classname.c_str(), method["name"].c_str());
}



#endif //STUB_GENERATOR_SERVERSTARTUP_H
