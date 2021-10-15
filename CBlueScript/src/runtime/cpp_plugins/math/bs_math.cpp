#define BUILD_DLL
#include "bs_math.h"

HEADER bsPower(bs_memory* mem, const string& args)
{
    vector<string> splitArgs = split(args, ",", true);
    bsMemoryObject base;
    float baseF = 0.0;
    float exp = 0.0;

    if (splitArgs[0].rfind("%", 0) == 0 && mem->varExists(split(splitArgs[0], "%")[1]))
    {
        base = mem->getVar(split(splitArgs[0], "%")[1]);
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

    if (splitArgs[1].rfind("%", 0) == 0 && mem->varExists(split(splitArgs[1], "%")[1]))
    {
        bsMemoryObject tail = mem->getVar(split(splitArgs[1], "%")[1]);
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

    mem->putObjInMemory(split(splitArgs[0], "%")[1], base);
}

HEADER bsMod(bs_memory* mem, const string& args)
{
    vector<string> splitArgs = split(args, ",", true);
    bsMemoryObject headVar;
    string tail = splitArgs[1];
    bool useFloat = false;

    if (splitArgs[0].rfind("%", 0) == 0 && mem->varExists(split(splitArgs[0], "%")[1]))
        headVar = mem->getVar(split(splitArgs[0], "%")[1]);
    
    else
    {
        cout << "Cannot operate on a non-variable\n";
        exit(6);
    }

    if (headVar.isConst)
    {
        cout << "Cannot change immutable value\n";
        exit(7);
    }
    
    if (tail.rfind("%", 0) == 0 && mem->varExists(split(tail, "%")[1]))
    {
        bsMemoryObject tmptail = mem->getVar(split(tail, "%")[1]);
        tail = tmptail.__str_reper__;

    }

    headVar.__str_reper__ = to_string(stol(headVar.__str_reper__) % stol(tail));
    mem->putObjInMemory(split(splitArgs[0], "%")[1], headVar);
    

}

HEADER bsSqrt(bs_memory* mem, const string& args)
{ 
    bsMemoryObject obj;
    if (args.rfind("%", 0) == 0 && mem->varExists(split(args, "%", true)[1]))
       obj = mem->getVar(split(args, "%", true)[1]);
    else
        obj = mem->createObject(args);

    float result;
    if (obj.isFloat)
        result = sqrt(stof(obj.__str_reper__));
    
    else
        result = sqrt(stol(obj.__str_reper__));

    int test = (int)result;
    if (test - result == 0)
    {
        obj.__str_reper__ = to_string((long) result);
        obj.isFloat = false;
    }
    else
    {
        obj.__str_reper__ = to_string(result);
        obj.isFloat = true;
    }

    mem->stackPush(obj);
}

HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
{
    map<string, void(*)(bs_memory* mem, const string& args)> retmap;

    retmap["pow"] = &bsPower;
    retmap["sqrt"] = &bsSqrt;
    retmap["bsmod"] = &bsMod;
    
    rt->insert(retmap.begin(), retmap.end());

}
