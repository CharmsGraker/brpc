#ifndef STUB_GENERATOR_UTILS_H
#define STUB_GENERATOR_UTILS_H

#include<cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cassert>
#include <fstream>
#include <cstdarg>
#include <iostream>
#include <cstring>

std::string getcwd();

std::string get_pdir(const std::string &path);

void mkdir(std::string &dir, int mode = 0777);

std::string getcwd();

std::string buildAbsPath(std::string path);

void mkdirs(std::string &dir);

bool exist_file(const std::string &path);

void remove_file(std::string &path);

void create_file(const std::string &path, bool exist_ok = false);

int write(const std::string &path, const std::string &content);

int append(std::fstream &f, const std::string &content);

int append(const std::string &path, const std::string &content);

template<typename ...Args>
std::string join(const std::string &s1, const std::string &s2, Args... args) {
    auto s = join(s1, s2);
    if (sizeof...(args) == 0)
        return s;
    return join(s, args...);
}

template<>
std::string join(const std::string &s1, const std::string &s2);

std::string get_pdir(const std::string &path);

std::string readFile(std::string path);

std::string format(const char *format, ...);

std::string getExt(std::string);

std::string getRelativeFromRoot(const std::string &root, const std::string &file_path);

#endif //STUB_GENERATOR_UTILS_H
