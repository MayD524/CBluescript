#pragma once

#ifndef TOOLKIT_H
#define TOOLKIT_H

#include <vector>
#include <string>
#include <map>

#include "UCPPL/UCPPL.h"
#include "bs_memory.h"

#ifdef _WIN32
    #ifndef BS_TOOLKIT_EXPORT
        #ifdef BUILD_DLL
            #define BS_TOOLKIT_EXPORT __declspec(dllexport)
        #else
            #pragma comment(lib, "toolkit.lib")
            #define BS_TOOLKIT_EXPORT __declspec(dllimport)
        #endif

        #define HEADER BS_TOOLKIT_EXPORT void __stdcall
#else
    #define HEADER void
#endif

#ifdef __cplusplus
    extern "C"
    {
#endif

    HEADER bs_STRCPY(bs_memory* mem, const string& args);
    HEADER bs_CONTAINS(bs_memory* mem, const string& args);
    HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt);

#ifdef __cplusplus
    }
#endif
#endif
#endif