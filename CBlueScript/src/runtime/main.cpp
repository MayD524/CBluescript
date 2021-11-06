/*
    Author: May Draskovics
    Date: 9/17/2021

    Description:
        The core file of BlueScript

    !!! CONTAINS MAIN !!!
    
*/

#define BLUE_SCRIPT

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "UCPPL/UCPPL.h"
#include "bs_runtime.h"

using namespace std;

map<string, void(*)(bs_memory* mem, const string& args)> linkedFuncs;

// for loading windows dlls
#if defined _WIN32
    #define OS "WIN32"
    #include <windows.h>

    typedef void (CALLBACK* LOAD_LIB)(map<string, void(*)(bs_memory* mem, const string& args)>* rt);

    HRESULT load_library( const string& path)
    {
        HINSTANCE hDLL;         // dll handler
        LOAD_LIB load_lib;      // function pointer
        HRESULT hrReturnVal;    // reuturn value
        hDLL = LoadLibrary(path.c_str());
        if (NULL == hDLL)
        {
            cout << "Could not load the library" << endl;
            hrReturnVal = ERROR_DELAY_LOAD_FAILED;
        }
        load_lib = (LOAD_LIB)GetProcAddress(hDLL, "load_lib");
        if (NULL == load_lib)
        {
            cout << "Could not find function" << endl;
            hrReturnVal = ERROR_DELAY_LOAD_FAILED;
        }
        else
        {
            load_lib(&linkedFuncs);
            hrReturnVal = 0;
        }
        return hrReturnVal;
    }

    void getlibs( const vector<string>& file_lines )
    {
        for (const string& line : file_lines)
        {
            if (line.rfind("#load_dll",0) == 0)
            {
                string dll_path = split(line, " ", true)[1];
                load_library(dll_path);
            }
        }
    }

// for handling linux .so files
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    #include <dlfcn.h>
    #define OS "LINUX"
    //typedef void (CALLBACK* LOAD_LIB)(map<string, void(*)(bs_runtime& runtime, string& args)>* rt);

    typedef void (*LOAD_LIB)(map<string, void(*)(bs_memory* mem, const string& args)>* rt);

    void load_library(const string& path)
    {
        char* error;
        void *handle = dlopen(path.c_str(), RTLD_LAZY);
        LOAD_LIB load_lib;

        if (!handle)
        {
            fprintf (stderr, "%s\n", dlerror());
            cout << "Could not load library" << endl;
            exit(1);
        }

        load_lib = (LOAD_LIB)dlsym(handle, "load_lib");
        if ((error = dlerror()) != NULL)
        {
            fprintf (stderr, "%s\n", error);
            exit(1);
        }

        (*load_lib)(&linkedFuncs);
        dlclose(handle);
        
    }

    void getlibs( const vector<string>& file_lines )
    {
        for (const string& line : file_lines)
        {
            if (line.rfind("#load_so",0) == 0)
            {
                string so_path = trim(split(line, " ", true)[1]);
                load_library(so_path);
            }
        }
    }

#endif

vector<string> argsToVector( char ** args )
{
    vector<string> returnVal; 
    for ( int i = 0; i < sizeof(args); i++)
    {
        if (args[i] == NULL) { return returnVal; }
        returnVal.push_back(args[i]);
    }
    return returnVal;
}

int main( int argc, char** argv )
{
    vector<string> args = argsToVector(argv);
    bool debugEnabled = false;
    bool autoWalk = false;
    if (argc > 0)
    {
        string fname = string(argv[1]);
        //cout << fname << endl;
        vector<string> fData = readFile(fname);

        if (anyInVector<string>("debug", args))
            debugEnabled = true;

        if (anyInVector<string>("self_walk", args))
            autoWalk = true;
        getlibs(fData);

        bs_runtime runtime = bs_runtime(fData, debugEnabled, linkedFuncs, autoWalk, args);


        runtime.runtimeMain();
    }
}