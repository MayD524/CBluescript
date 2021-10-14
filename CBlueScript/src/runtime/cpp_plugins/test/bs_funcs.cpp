#include "bs_funcs.h"

void bs_MOV(bs_memory& mem, const string& cmd_args)
{
    vector<string> arg_parse = split(cmd_args, ",", true);
    mem.bsMovCmd(arg_parse[0], arg_parse[1]);
    return;
}
void bs_RINT(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    string range_min = splitArgs[0];
    string range_max = splitArgs[1];
    long rangeMin = BLUESCRIPT_RAND_DEFAULTMIN;
    long rangeMax = BLUESCRIPT_RAND_DEFAULTMAX;

    if (range_min.rfind("%",0) == 0)
    {
        bsMemoryObject minObj = mem.getVar(split(range_min, "%", true)[1]);
        range_min = minObj.__str_reper__;
    }

    if (range_max.rfind("%",0) == 0)
    {
        bsMemoryObject maxObj = mem.getVar(split(range_max, "%", true)[1]);
        range_max = maxObj.__str_reper__;
    }

    rangeMin = stoi(range_min);
    rangeMax = stoi(range_max);
                
    // get the random number
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> dist6(rangeMin, rangeMax);

    long val = dist6(rng);
                
    // push to stack
    bsMemoryObject valObj = mem.createObject(to_string(val));
    mem.stackPush(valObj);
}
void bs_REM(bs_memory& mem, const string& cmd_args)
{
    if (cmd_args.rfind("%", 0) == 0)
        mem.varRemove(split(cmd_args, "%")[1]);
                
    else
    {
        cout << "variable " << cmd_args << " does not exist.\n";
        exit(23);
    }
}
void bs_GETTYPE(bs_memory& mem, const string& cmd_args)
{
    vector<string> argSplit = split(cmd_args, ",");
    if (argSplit[1].rfind("%",0) == 0)
    {
        bsMemoryObject obj = mem.getVar(split(cmd_args, "%")[1]);
        mem.bsMovCmd(argSplit[0], obj.__str_reper__);
    }
    else
    {
        cout << "Variable " << argSplit[1] << " does not exist or hasn't been defined.\n";
       exit(23);
    }
}

void bs_INDEX(bs_memory& mem, const string& cmd_args)
{
    vector<string> argSplit = split(cmd_args, ",", true);
    vector<string> varNameAndIndex = split(argSplit[1], "["); // get the index
    string index = split(varNameAndIndex[1], "]")[0];
    long getIndex = 0;

    if (index.rfind("%", 0) == 0)
    {
        bsMemoryObject indexObj = mem.getVar(split(index, "%")[1]);
        getIndex = stol(indexObj.__str_reper__);
    }
    else
    {
        getIndex = stol(index);
    }

    if (varNameAndIndex[0].rfind("%", 0) == 0)
    {
        bsMemoryObject lookfor = mem.getVar(split(varNameAndIndex[0], "%")[1]);
        mem.bsMovCmd(argSplit[0], string(1, lookfor.__str_reper__[getIndex]));
    }
}

void bs_SET(bs_memory& mem, const string& cmd_args)
{
    vector<string> argSplit = split(cmd_args, ",", true);

    if (argSplit[0].compare("isConst") == 0)
    {
        if (argSplit[1].compare("1") == 0)
            mem.constFlag = true;
        else
            mem.constFlag = false;
    }
}
void bs_SIZEOF(bs_memory& mem, const string& cmd_args)
{
    vector<string> argSplit = split(cmd_args, ",", true);
    if (argSplit[1].rfind("%", 0) == 0)
    {
        bsMemoryObject tmpMem = mem.getVar(split(argSplit[1], "%")[1]);
        unsigned long varSize = tmpMem.size;
        mem.bsMovCmd(argSplit[0], to_string(varSize));
    }
    else
    {
        printf("The variables %s does not exist", argSplit[1].c_str());
        exit(20);
    }
}
void bs_READ(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    string filename = splitArgs[1];
    string outVar = splitArgs[0];

    if (filename.rfind("%",0) == 0)
    {
        bsMemoryObject fnameObj = mem.getVar(split(filename, "%", true)[1]);
        filename = fnameObj.__str_reper__;
    }

    vector<string> readFileData = readFile(filename);
    mem.createArray(outVar, readFileData.size());

    for (int i = 0; i < readFileData.size(); i++)
    {
        bsMemoryObject lineObj = mem.createObject(readFileData[i]);
        mem.arrayAppend(outVar, i, lineObj);
    }
    bsMemoryObject arrayNameObj = mem.createObject(outVar);
    mem.putObjInMemory(outVar, arrayNameObj);
}
void bs_WRITE(bs_memory& mem, const string& cmd_args)
{
    vector<string> splitArgs = split(cmd_args, ",", true);
    string filename = splitArgs[0];
    string arrayName = splitArgs[1];

    if (filename.rfind("%", 0) == 0)
    {
        bsMemoryObject fnameObj = mem.getVar(split(filename, "%")[1]);
        filename = fnameObj.__str_reper__;
    }
    if (arrayName.rfind("%", 0) == 0)
    {
        bsMemoryObject aNameObj = mem.getVar(split(arrayName, "%", true)[1]);
        arrayName = aNameObj.__str_reper__;
    }

    ofstream file_out;
    file_out.open(filename, ios_base::app);

    for (int i = 0; i < mem.arraySize(arrayName); i++)
    {
        bsMemoryObject arrayObj = mem.getFromArray(arrayName, i);
        file_out << arrayObj.__str_reper__;
    }
    file_out.close();
}