/*
    Author: May Draskovics
    Date: 10/14/2021

    Description:
        Handle arrays in bluescript
*/


#include "bs_arrayTools.h"

void bs_ARRAY_SIZE(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    string returnVar = splitArgs[0];
    string arrayName = splitArgs[1];

    if (arrayName.rfind("%", 0) == 0)
    {
        bsMemoryObject arrayNameObj = mem.getVar(split(arrayName, "%")[1]);
        arrayName = arrayNameObj.__str_reper__;
    }

    mem.bsMovCmd(returnVar, to_string(mem.arraySize(arrayName)));
}

void bs_ARRAY_ERASE(bs_memory& mem, const string& cmd_args)
{
    string arr = cmd_args;
    if (cmd_args.rfind("%", 0) == 0)
    {
        bsMemoryObject arrayName = mem.getVar(split(cmd_args, "%")[1]);
        arr = arrayName.__str_reper__;
    }

    mem.eraseArray(arr);
}

void bs_ARRAY_MAKE(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    string arrayName;
    int arraySize;

    if (splitArgs[0].rfind("%", 0) == 0)
    {
        bsMemoryObject arrayNameObj = mem.getVar(split(splitArgs[0], "%")[1]);
        arrayName = arrayNameObj.__str_reper__;
    }
    else
        arrayName = splitArgs[0];

    if (splitArgs[1].rfind("%", 0) == 0)
    {
        bsMemoryObject arraySizeObj = mem.getVar(split(splitArgs[1], "%")[1]);
        arraySize = stoi(arraySizeObj.__str_reper__);
    }
    else
        arraySize = stoi(splitArgs[1]);

    mem.createArray(arrayName, arraySize);
}

void bs_ARRAY_GET(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    vector<string> arrayNameIndex = split(splitArgs[1], "[");
    string outVar = splitArgs[0];
    string arrayName = arrayNameIndex[0];
    string arrayIndex = split(arrayNameIndex[1], "]")[0];

    if (arrayName.rfind("%", 0) == 0)
    {
        bsMemoryObject arrNameObj = mem.getVar(split(arrayName, "%")[1]);
        arrayName = arrNameObj.__str_reper__;
    }

    if (arrayIndex.rfind("%", 0) == 0)
    {
        bsMemoryObject arrIndexObj = mem.getVar(split(arrayIndex, "%")[1]);
        arrayIndex = arrIndexObj.__str_reper__;
    }

    bsMemoryObject arrayGet = mem.getFromArray(arrayName, stoi(arrayIndex));

    mem.putObjInMemory(outVar, arrayGet);
}

void bs_ARRAY_SET(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    vector<string> arrayNameIndex = split(splitArgs[0], "[");
    string InData = splitArgs[1];
    string arrayName = arrayNameIndex[0];
    string arrayIndex = split(arrayNameIndex[1], "]")[0];

    if (InData.rfind("%", 0) == 0)
    {
        bsMemoryObject indataObj = mem.getVar(split(InData, "%")[1]);
        InData = indataObj.__str_reper__;
    }

    if (arrayName.rfind("%", 0) == 0)
    {
        bsMemoryObject arrayNameObj = mem.getVar(split(arrayName, "%")[1]);
        arrayName = arrayNameObj.__str_reper__;
    }

    if (arrayIndex.rfind("%", 0) == 0)
    {
        bsMemoryObject arrayIndexObj = mem.getVar(split(arrayIndex, "%")[1]);
        arrayIndex = arrayIndexObj.__str_reper__;
    }
    mem.arrayAppend(arrayName, stoi(arrayIndex), mem.createObject(InData));
}
