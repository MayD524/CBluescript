#define BUILD_DLL

#include "toolkit.h"

HEADER bs_STRCPY(bs_memory* mem, const string& args)
{
    vector<string> splitArgs = split(args, ",");
    string base = splitArgs[0];
    string strCopy = splitArgs[1];
    string count = splitArgs[2];
    
    if (base.rfind("%", 0) == 0 && mem->varExists(split(base, "%", true)[1]))
    {
        bsMemoryObject baseVar = mem->getVar(split(base, "%", true)[1]);
        base = baseVar.__str_reper__;
    }
    if (strCopy.rfind("%", 0) == 0 && mem->varExists(split(strCopy, "%", true)[1]))
    {
        bsMemoryObject strCopyVar = mem->getVar(split(strCopy, "%", true)[1]);
        strCopy = strCopyVar.__str_reper__;
    }
    if (count.rfind("%", 0) == 0 && mem->varExists(split(count, "%", true)[1]))
    {
        bsMemoryObject countVar = mem->getVar(split(count, "%", true)[1]);
        count = countVar.__str_reper__;
    }

    long cnt = stol(count);
    for (long i = 0; i < cnt; i++)
    {
        base += strCopy;
    }

    bsMemoryObject outVar = mem->createObject(base);
    mem->stackPush(outVar);

}

HEADER bs_CONTAINS(bs_memory* mem, const string& args)
{
    vector<string> splitArgs = split(args, ",", true);
    string str = splitArgs[0];
    string substr = splitArgs[1];

    if (str.rfind("%", 0) == 0 && mem->varExists(split(str, "%", true)[1]))
    {
        bsMemoryObject strObj = mem->getVar(split(str, "%", true)[1]);
        str = strObj.__str_reper__;
    }

    if (substr.rfind("%", 0) == 0 && mem->varExists(split(substr, "%", true)[1]))
    {
        bsMemoryObject substrObj = mem->getVar(split(substr, "%", true)[1]);
        substr = substrObj.__str_reper__;
    }

    bsMemoryObject outObj;

    if(str.find(substr) != string::npos)
        outObj = mem->createObject("1");
    else
        outObj = mem->createObject("0");
    
    mem->stackPush(outObj);
}

HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
{
    map<string, void(*)(bs_memory* mem, const string& args)> retmap;

    retmap["strcpy"] = &bs_STRCPY;
    retmap["contains"] = &bs_CONTAINS;

    rt->insert(retmap.begin(), retmap.end());
}