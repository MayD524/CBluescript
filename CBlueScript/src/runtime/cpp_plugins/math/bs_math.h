#pragma once

#ifndef BS_MATH_H
#define BS_MATH_H

#include <math.h>
#include <string>
#include <map>

#include "bs_memory.h"

using namespace std;

#ifdef _WIN32
    #ifndef BS_MATH_EXPORT
        #ifdef BUILD_DLL
            #define BS_MATH_EXPORT __declspec(dllexport)
        #else
            #pragma comment(lib, "bs_math.lib")
            #define BS_MATH_EXPORT __declspec(dllimport)
        #endif
    #endif
    #define HEADER BS_MATH_EXPORT void __stdcall
    
#else
    #define HEADER void

#endif
#ifdef __cplusplus
    extern "C"
    {
#endif
    HEADER bsMod(bs_memory* mem, const string& args);
    HEADER bsSqrt(bs_memory* mem, const string& args);
    HEADER bsPower(bs_memory* mem, const string& args);
    HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt);
#ifdef __cplusplus
    }
#endif


#endif