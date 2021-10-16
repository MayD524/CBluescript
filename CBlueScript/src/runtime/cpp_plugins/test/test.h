#pragma once

#ifndef TEST_H
#define TEST_H
#include <string>
#include <map>
#include "bs_memory.h"

using namespace std;

#ifdef _WIN32
    #ifndef TEST_EXPORTS
        #ifdef BUILD_DLL
            #define TEST_EXPORTS __declspec(dllexport)
        #else
        #pragma comment(lib, "test.lib")
            #define TEST_EXPORTS __declspec(dllimport)
        #endif
    #endif

    #ifdef __cplusplus
        extern "C"
        {
    #endif
        TEST_EXPORTS void __stdcall helloWorld(bs_memory* mem, const string& args);
        TEST_EXPORTS void __stdcall load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt);
    #ifdef __cplusplus
        }
    #endif

    #endif
#else
    #ifdef __cplusplus
        extern "C"
        {
    #endif
        void helloWorld(bs_runtime* runtime, string& args);
        void load_lib(map<string, void(*)(bs_runtime* runtime, string& args)>* rt);
    #ifdef __cplusplus
        }
    #endif
#endif