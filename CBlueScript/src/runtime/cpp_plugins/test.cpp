#define BUILD_DLL
#include "test.h"
#ifdef _WIN32

    extern "C"
    {
        TEST_EXPORTS void __stdcall helloWorld(bs_runtime* runtime, string& args)
        {
            if (args.rfind("%",0) == 0)
            {
                printf("|%s|\n", trim(split(args, "%")[1]).c_str());
                bsMemoryObject argObj = runtime->memoryObject.getVar(trim(split(args, "%")[1]));
                cout << argObj.isObjectOf << endl;
                cout << runtime->memoryObject.getStringReperOfVariable(argObj.isObjectOf, argObj.obj) << endl;
            }
            else
            {
                cout << args << endl;
            }
        }

        TEST_EXPORTS void __stdcall load_lib(map<string, void(*)(bs_runtime* runtime, string& args)>* rt)
        {
            map<string, void(*)(bs_runtime* runtime, string& args)>retmap;

            retmap["hello_world2"] = &helloWorld;

            rt->insert(retmap.begin(), retmap.end());
        }
    }

#else
    extern "C"
    {
        void helloWorld(bs_runtime& runtime, string& args)
        {
            cout << "hello world" << endl;
        }

        void load_lib(map<string, void(*)(bs_runtime& runtime, string& args)>* rt)
        {
            map<string, void(*)(bs_runtime& runtime, string& args)>retmap;

            retmap["hello_world2"] = &helloWorld;

            rt->insert(retmap.begin(), retmap.end());
        }
    }
#endif