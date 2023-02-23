//
// Created by nekonoyume on 2023/2/20.
//

#ifndef STUB_GENERATOR_SERVERSTARTUP_H
#define STUB_GENERATOR_SERVERSTARTUP_H

#include "classproxy.h"
#include "code_analyser/headerparser.h"


namespace rpc_config {
    extern std::string project_root;
}

namespace stub_config {
    extern std::string main_header_path;
    extern std::string buttonrpc_header_path;
    extern std::string class_src_main_header_path;
}

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
    bool build = false;
public:
    ServerStartup() = delete;

    ServerStartup(const std::string &root) : server_root(root) {};

public:
    void buildAll() {
        using stub_config::buttonrpc_header_path;
        using stub_config::main_header_path;

        assert(!server_root.empty());
        auto startup_file_path = server_root + "/rpc_build/start_server.cpp";
        auto tmp_file_path = server_root + "/rpc_build/start_server_tmp.cpp";
        std::stringstream ss;
        std::smatch sm;
        std::regex ra_re("// @bind_area");

        if (!exist_file(startup_file_path)) {
            create_file(startup_file_path);
            start_area.insert(format("#include \"%s\"", buttonrpc_header_path.c_str()));
            start_area.insert(format("#include \"%s\"", main_header_path.c_str()));
            ss << start_area.render();
            ss << readFile(server_root + "/conf/run.template");
        } else
            ss << readFile(startup_file_path);
        std::ofstream tmp_file(tmp_file_path, std::ios::out);
        std::string buffer;
        bool write_before = false;
        while (getline(ss, buffer, '\n')) {
            std::regex_search(buffer, sm, ra_re);
            if (!sm.empty() && !write_before) {
                write_before = true;
                tmp_file << run_area.render();
            }
            tmp_file << buffer << "\n";
        }
        tmp_file.close();
        rename(tmp_file_path.c_str(), startup_file_path.c_str());
        build = true;
    }

    void addRPCMethod(std::string &rpcName, Method &method, const std::string &classname = "") {
        build = false;
        std::string stat;
        if (classname.empty()) {
            stat = format("server.bind(\"%s\", %s);",
                          rpcName.c_str(),
                          method["name"].c_str());
        } else {
            stat = format("server.bind(\"%s\", &%s::%s, (%s *)%s);",
                          rpcName.c_str(),
                          classname.c_str(),
                          method["name"].c_str(),
                          classname.c_str(),
                          "nullptr");
        }
        run_area.insert(stat);
    }

    void addSourcesOf(Clazz &clazz);

    void addSourcesOf(Header &header);

    ~ServerStartup() {
        if (!build)
            buildAll();
    }
};

std::string buildRPCName(const Clazz &clazz, Method method) {
    return format("%s::%s", clazz.classname.c_str(), method["name"].c_str());
}

std::string buildRPCName(const Header &header, Method method) {
    return format("%s::%s", getRelativeFromRoot(rpc_config::project_root, header.getPath()).c_str(),
                  method["name"].c_str());
}

void ServerStartup::addSourcesOf(Clazz &clazz) {
    const std::map<std::string, Method> &methods = clazz.methods;
    using stub_config::class_src_main_header_path;
    {
        if (!exist_file(class_src_main_header_path))
            create_file(class_src_main_header_path);
        CodeGenerator gen;
        gen.addInclude(clazz.getPath());
        gen.setOutputPath(class_src_main_header_path);
        gen.render(std::ios::app);
    }
    for (auto &&[id, m]: methods) {
        if (clazz.classname == m.getName() || m.hasTemplateArg())
            continue;
        auto m_rpcname = buildRPCName(clazz, m);
        addRPCMethod(m_rpcname, const_cast<Method &> (m), clazz.classname);
    }
}

void ServerStartup::addSourcesOf(Header &header) {
    const std::map<std::string, Method> &methods = header.methods;
    for (auto &[id, m]: methods) {
        auto m_rpcname = buildRPCName(header, const_cast<Method &> (m));
        addRPCMethod(m_rpcname, const_cast<Method &>(m));
    }
}

#endif //STUB_GENERATOR_SERVERSTARTUP_H
