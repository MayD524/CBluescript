#define BUILD_DLL
#include "test.h"
#ifdef _WIN32

    extern "C"
    {
        TEST_EXPORTS void __stdcall helloWorld(bs_memory* mem, const string& args)
        {
            if (args.rfind("%", 0) == 0)
            {
                bsMemoryObject tmp = mem->getVar(split(args, "%", true)[1]);
                cout << tmp.__str_reper__ << endl;
            }
            else
                cout << args << endl;
        }

        TEST_EXPORTS void __stdcall load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
        {
            map<string, void(*)(bs_memory* mem, const string& args)>retmap;

            retmap["hello_world2"] = &helloWorld;

            rt->insert(retmap.begin(), retmap.end());
        }
    }

#else
    extern "C"
    {
        void helloWorld(const bs_runtime& runtime, const string& args)
        {
            cout << "hello world" << endl;
        }

        void load_lib(map<string, void(*)(const bs_runtime& runtime, const string& args)>* rt)
        {
            map<string, void(*)(bs_runtime& runtime, string& args)>retmap;

            retmap["hello_world2"] = &helloWorld;

            rt->insert(retmap.begin(), retmap.end());
        }
    }
#endif