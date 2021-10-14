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

    // stop name mangling
    #ifdef __cplusplus
        extern "C"
        {
    #endif

        BS_MATH_EXPORT void __stdcall bsMod(bs_memory* mem, const string& args);
        BS_MATH_EXPORT void __stdcall bsSqrt(bs_memory* mem, const string& args);
        BS_MATH_EXPORT void __stdcall bsPower(bs_memory* mem, const string& args);
        BS_MATH_EXPORT void __stdcall load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt);
        
    #ifdef __cplusplus
        }
    #endif

#endif


#endif