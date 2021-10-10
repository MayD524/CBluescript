#include "test.h"
#ifdef _WIN32
    #define BUILD_DLL

    extern "C"
    {
        TEST_EXPORTS void helloWorld(bs_runtime& runtime, string& args)
        {
            cout << "hello world" << endl;
        }

        TEST_EXPORTS void __stdcall load_lib(map<string, void(*)(bs_runtime& runtime, string& args)>* rt)
        {
            map<string, void(*)(bs_runtime& runtime, string& args)>retmap;

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