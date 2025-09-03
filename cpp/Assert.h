#pragma once
#include <cassert>
#include <iostream>
#include <cstdarg>
#include <cstdio>

inline void assert_fail_msg(const char* file, int line, const char* cond, const char* fmt = nullptr, ...) {
    std::cerr << "Assertion failed: ";
    if (fmt) {
        char buffer[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        std::cerr << buffer << " ";
    }
    std::cerr << "(" << cond << ") at " << file << ":" << line << std::endl;
}

#define ASSERT(cond, ...) \
    do { \
        if (!(cond)) { \
            assert_fail_msg(__FILE__, __LINE__, #cond, ##__VA_ARGS__); \
            assert(cond); \
        } \
    } while(0)
