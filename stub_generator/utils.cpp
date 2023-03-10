#include "utils.h"
#include "../lib/string_utils/string_utils.h"

#include<cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <fstream>
#include <cstdarg>
#include <iostream>
#include <cstring>
#include <regex>

std::string getcwd() {
    char *buffer = NULL;
    // 区分此函数是在Windows环境调用还是Linux环境调用
#if defined (_WIN64) || defined (WIN32) || defined (_WIN32)
    // 获取项目的工作路径
    buffer = _getcwd(NULL, 0);
#else
    // 获取项目的工作路径
    buffer = getcwd(NULL, 0);
#endif
    if (buffer) {
        std::string path = buffer;
        free(buffer);
        return path;
    }
    return "";
}


std::string buildAbsPath(std::string path) {
    if (path.empty())
        return ".";
    if (path[0] != '/') {
        auto pwd = getcwd();
        path = pwd + "/" + path;
    }
    auto a = split(path, '/');
    std::vector<std::string> ret;
    int len = a.size();
    for (int i = 0; i < len; ++i) {
        if (a[i] == ".") {
            continue;
        } else if (a[i] == "..") {
            assert(!ret.empty()); // illegal path
            ret.pop_back();
        } else {
            ret.push_back(a[i]);
        }
    }

    if (ret.empty())
        return "/";

    len = ret.size();
    std::string ans;
    for (int i = 0; i < len; ++i) {
        ans += '/';
        ans += ret[i];
    }
    return ans;
}

void mkdir(std::string &dir, int mode) {
    auto ret = ::mkdir(dir.c_str(), mode);
}


void mkdirs(std::string &dir) {
    auto len = dir.length();
    for (int i = 0; i < len; ++i) {
        if (dir[i] == '/') {
            auto d = dir.substr(0, i);
            if (access(d.c_str(), NULL))
                mkdir(d);
        }
    }
    if (len && access(dir.c_str(), NULL))
        mkdir(dir);
}

bool exist_file(const std::string &path) {
    auto fp = fopen(path.c_str(), "r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

void remove_file(std::string &path) {
    remove(path.c_str());
}

void create_file(const std::string &path, bool exist_ok) {
    if (!exist_ok && exist_file((path)))
        return;
    auto pdir = get_pdir(path);
    mkdirs(pdir);
    auto fp = fopen(path.c_str(), "w+");
    assert(fp);
    fclose(fp);
}


int write(const std::string &path, const std::string &content) {
    if (!exist_file(path))
        return -1;
    std::ofstream f(path);
    f << content;
    f.close();
}

int append(std::fstream &f, const std::string &content) {
    f << content;
}

int append(const std::string &path, const std::string &content) {
    if (!exist_file(path))
        return -1;
    std::ofstream f(path, std::ios::app);
    f << content;
    f.close();
}


template<>
std::string join(const std::string &s1, const std::string &s2) {
    return s1 + "/" + s2;
}

std::string get_pdir(const std::string &path) {
    auto j = path.rfind('/');
    if (j == path.npos) {
        // do not exist parent dir or fatal error
        return {};
    }
    return path.substr(0, j);
}

std::string readFile(std::string path) {
    std::ifstream f;
    std::cout << "read file from: " << path << std::endl;
    f.open(path);
    std::string line;
    std::string content;
    while (getline(f, line, '\n')) {
        content += line;
        content += '\n';
    }
    f.close();
    return content;
}

std::string format(const char *format, ...) {
    va_list args;
    char buffer[2048];
    memset(buffer, 0, sizeof buffer);
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    return {buffer};
}

std::string getExt(std::string path) {
    auto pos = path.rfind('.');
    assert(pos != path.npos);
    return path.substr(pos);
}

std::string getRelativeFromRoot(const std::string &root, const std::string &file_path) {
    auto root_abs = buildAbsPath(root);
    auto f_abs = buildAbsPath(file_path);
    std::string::const_iterator beg = f_abs.cbegin(), end = f_abs.cend();
    std::smatch sm;
    std::string ret;
    // remove extension
    std::regex re(root_abs + "(.*?)(\\.(h|cpp|cc|hpp))");
    std::regex_match(beg, end, sm, re);
    assert(!sm.empty());
    return sm[1].str();
}