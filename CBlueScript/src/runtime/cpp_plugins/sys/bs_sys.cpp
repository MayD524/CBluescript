#define BUILD_DLL
#include "bs_sys.h"

HEADER ARGV_INIT(bs_memory* mem, const string& args)
{
    
}

HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
{
    map<string, void(*)(bs_memory* mem, const string& args)> retmap;
    
    rt->insert(retmap.begin(), retmap.end());

}
