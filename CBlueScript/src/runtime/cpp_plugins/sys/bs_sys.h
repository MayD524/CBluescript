#pragma once

#ifndef BS_MATH_H
#define BS_MATH_H

#include <math.h>
#include <string>
#include <map>

#include "bs_memory.h"

using namespace std;

#ifdef _WIN32
    #define OS_NAME "WINDOWS"
    #ifndef BS_SYS_EXPORT
        #ifdef BUILD_DLL
            #define BS_SYS_EXPORT __declspec(dllexport)
        #else
            #pragma comment(lib, "bs_sys.lib")
            #define BS_SYS_EXPORT __declspec(dllimport)
        #endif
    #endif
    #define HEADER BS_SYS_EXPORT void __stdcall
    
#else
    #define OS_NAME "POSIX"
    #define HEADER void

#endif
#ifdef __cplusplus
    extern "C"
    {
#endif
    // file system stuff
    HEADER fileExists(bs_memory* mem, const string& args);
    HEADER dirExists(bs_memory* mem, const string& args);
    HEADER exists(bs_memory* mem, const string& args);

    // os related items
    HEADER osName(bs_memory* mem, const string& args);
    HEADER sysInfo(bs_memory* mem, const string& args);

    // load the lib
    HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt);

#ifdef __cplusplus
    }
#endif


#endif