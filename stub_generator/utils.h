#ifndef STUB_GENERATOR_UTILS_H
#define STUB_GENERATOR_UTILS_H

#include<cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

std::string getcwd();

std::string get_pdir(const std::string &path);

void mkdir(std::string &dir, int mode = 0777);

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

void mkdir(std::string &dir, int mode) {
    auto ret = ::mkdir(dir.c_str(), mode);
}

void mkdir(const std::string &dir, int mode) {
    mkdir(const_cast<std::string &>(dir), mode);
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

void create_file(const std::string &path, bool exist_ok = false) {
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

template<typename ...Args>
std::string join(const std::string &s1, const std::string &s2, Args... args) {
    auto s = join(s1, s2);
    if (sizeof...(args) == 0)
        return s;
    return join(s, args...);
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
    try {
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
    } catch (std::exception &e) {
        std::cout << e.what();
    }
    return {};
}

std::string format(const char *format, ...) {
    va_list args;
    char buffer[2048];
    memset(buffer, 0, sizeof buffer);
    va_start(args,format);
    vsprintf(buffer, format, args);
    va_end(args);
    return {buffer};
}

#endif //STUB_GENERATOR_UTILS_H
