#define BUILD_DLL
#include "bs_math.h"

BS_MATH_EXPORT void __stdcall bsPower(bs_memory* mem, const string& args)
{
    vector<string> splitArgs = split(args, ",", true);
    bsMemoryObject base;
    float baseF = 0.0;
    float exp = 0.0;

    if (mem->varExists(splitArgs[0]))
    {
        base = mem->getVar(splitArgs[0]);
        baseF = stof(base.__str_reper__);
    }
    else
    {
        cout << "Cannot operate on a non-variable" << endl;
        exit(6);
    }

    if (base.isConst)
    {
        cout << "Cannot change immutable varibale" << endl;
        exit(7);
    }

    if (mem->varExists(splitArgs[1]))
    {
        bsMemoryObject tail = mem->getVar(splitArgs[1]);
        exp = stof(tail.__str_reper__);
    }
    else
        exp = stof(splitArgs[1]);

    baseF = pow(baseF, exp);

    int test = (int) baseF;

    if (baseF - test != 0)
    {
        base.isFloat = true;
        base.__str_reper__ = to_string(baseF);
    }
    else
    {
        // use test to save time (already int)
        base.isFloat = false;
        base.__str_reper__ = to_string(test);
    }

    base.size = base.__str_reper__.size();

    mem->putObjInMemory(splitArgs[0], base);
}

BS_MATH_EXPORT void __stdcall bsMod(bs_memory* mem, const string& args)
{

}

BS_MATH_EXPORT void __stdcall bsSqrt(bs_memory* mem, const string& args)
{

}
BS_MATH_EXPORT void __stdcall load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
{
    map<string, void(*)(bs_memory* mem, const string& args)> retmap;

    retmap["pow"] = &bsPower;

    rt->insert(retmap.begin(), retmap.end());

}