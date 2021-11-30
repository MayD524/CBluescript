
#include "bs_memory.h"

bsMemoryObject bs_memory::bs_NULL( void )
{
    bsMemoryObject nll;
    nll.isConst = true;
    nll.dType = BS_NULL;
    nll.__str_reper__ = "NULL";

    return nll;
}

string bs_memory::stringReperOfType( Types type )
{
    switch (type)
    {
        case BS_String: return "string";
        case BS_Char: return "char";
        case BS_Num: return "numeric";
        case BS_Bool: return "bool";
        case BS_Bytes: return "bytes";
        case BS_Array: return "array";
        case BS_NULL: return "NULL";
        default: return "object";
    }
}

int bs_memory::getType( const string& _value )
{
    if (_value.compare("true") == 0 || _value.compare("false") == 0)
        return BS_Bool;
    
    else if (is_number(_value))
        return BS_Num;
    
    else if (_value == "NULL")
        return BS_NULL;
    
    else if (_value.rfind("0b", 0) == 0)
        return BS_Bytes;
    
    else
        return BS_String;
}

void bs_memory::getAllKeys( void )
{
    for (map<string,bsMemoryObject>::iterator iter = this->bsMemory.begin(); iter != this->bsMemory.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second.__str_reper__;
        printf("%s:%s\n", key.c_str(), value.c_str());
    }
}

void bs_memory::stackPush( const bsMemoryObject& _value )
{
    this->bsStack.push_back(_value); 
}

void bs_memory::eraseArray( const string& arrayName )
{
    auto curArr = this->bsArraySizes.find(arrayName);
    if (curArr == this->bsArraySizes.end())
        return; // nothing to do
            
    ArrayLocs curArrSizes = curArr->second;

    // erase all data
    for (int i = curArrSizes.start; i < curArrSizes.end; i++)
        this->arrayStack.erase(this->arrayStack.begin() + i);
    this->bsArraySizes.erase(arrayName);
    int total_size = curArrSizes.start + curArrSizes.end;
    for (map<string, ArrayLocs>::iterator iter= this->bsArraySizes.begin(); iter != this->bsArraySizes.end(); iter++)
    {
        ArrayLocs iterLocs = iter->second;

        if (iterLocs.end < curArrSizes.start)
            continue;
                
        iterLocs.start -= total_size;
        iterLocs.end -= total_size;
    }
}

void bs_memory::displayArray( void )
{
    for (bsMemoryObject& o : this->arrayStack)
    {
        cout << o.__str_reper__ << endl;
    }
}

void bs_memory::createArray( const string& arrayName, const int& arraySize)
{
    ArrayLocs newArray;

    newArray.start = this->arrayStack.size();
    newArray.end = newArray.start + arraySize;
    // init the array
    for (int i = 0; i < arraySize; i++)
    {
        this->arrayStack.push_back(this->bs_NULL());
    }
    this->bsArraySizes.insert({ arrayName, newArray });
}

void bs_memory::arrayAppend( const string& arrayName, int index, const bsMemoryObject& obj )
{
    index = this->indexInArray(arrayName, index);
    if (index > -1)
    {
        this->arrayStack[index] = obj;
        return;
    }
    printf("[bs_memory::arrayAppend] Error out of range (%i is out of range for %s)", index, arrayName.c_str());
    exit(21);
}

void bs_memory::varRemove( const string& _varptr )
{
    if (this->varExists(_varptr))
        this->bsMemory.erase(_varptr);
}

void bs_memory::putObjInMemory( const string& _ptrname, const bsMemoryObject& _data)
{
    if (this->varExists(_ptrname))
    {
        if ( this->getVar(_ptrname).isConst == true)
        {
            bsMemoryObject currentVal = this->getVar(_ptrname);
            if (currentVal.__str_reper__ != _data.__str_reper__)
            {
                printf("[bs_memory::putObjInMemory]Cannot change an immutable variable \"%s\"\n", _ptrname.c_str());
                exit(1);
            }
            else
                return;
        }
        else
            this->bsMemory.erase(_ptrname);
    }

    this->bsMemory.insert({ _ptrname, _data });
    return;
}

void bs_memory::bsMovCmd( const string& _ptrname, const string& _value)
{
    bsMemoryObject memObject;
    if (_value.rfind("%", 0) == 0)
        memObject = this->getVar(split(_value, "%")[1]);
    else
        memObject = this->createObject(_value);
            
    this->putObjInMemory( _ptrname, memObject );
}

int bs_memory::arraySize( const string& arrayName )
{
    auto it = this->bsArraySizes.find(arrayName);
            
    if (it == this->bsArraySizes.end())
    {
        printf("[bs_memory::arraySize]Array does not exist. \"%s\"\n", arrayName.c_str());
        exit(23);
        return -1;
    }

    ArrayLocs locs = it->second;
    return locs.start + locs.end;
}

int bs_memory::indexInArray( const string& arrayName, int index )
{
    auto it = this->bsArraySizes.find(arrayName);
    if (it == this->bsArraySizes.end())
    {
        printf("[bs_memory::indexInArray]Array does not exist. \"%s\"\n", arrayName.c_str());
        exit(24);
        return -1;
    }
    ArrayLocs locs = it->second;
    index += locs.start;

    if (index >= locs.start && index <= locs.end)
    {
        return index;
    }
    return index;
}

bool bs_memory::varExists( const string& _ptrname )
{
    auto it = this->bsMemory.find(_ptrname);

    if (it != this->bsMemory.end())
        return true;

    return false;
}

bsMemoryObject bs_memory::getFromArray( const string& arrayName, int index)
{
    index = this->indexInArray(arrayName, index);
    if (index > -1)
    {
        return this->arrayStack[index];
    }

    printf("[bs_memory::getFromArray] Out of range error (%i is out of range for %s)\n", index, arrayName.c_str());
    exit(21);

    // just so c++ can compile
    return this->bs_NULL();
}

template<typename T>
T bsMath(T head, T tail, const int& oper)
{
    switch (oper)
    {
        case BS_ADD: return head + tail;
        case BS_SUB: return head - tail;
        case BS_DIV: return head / tail;
        case BS_MUL: return head * tail;
        default: return head + tail;
    }
}

bsMemoryObject bs_memory::runMath( const string& line, const int bsMathOper )
{
    bsMemoryObject outObj;

    bsMemoryObject headVar;
    bsMemoryObject tailVar;

    outObj.dType = BS_Num;
    outObj.isConst = this->constFlag;
    this->constFlag = false;

    vector<string> args = split(line, ",", true);
    if (this->varExists(args[0]))
    {
        headVar = this->getVar(args[0]);

        if (headVar.isConst)
        {
            printf("The variable '%s' is immutable\n", args[0].c_str());
            exit(1);
        }
    }
    else
        tailVar = this->createObject(args[1]);

    if (args[1].rfind("%",0) == 0 && this->varExists(split(args[1], "%")[1]))
        tailVar = this->getVar(split(args[1], "%")[1]);
    else
        tailVar = this->createObject(args[1]);

    if (headVar.isFloat || tailVar.isFloat)
    {
        float out = bsMath<float>(stof(headVar.__str_reper__), stof(tailVar.__str_reper__), bsMathOper);
        outObj.isFloat = true;
        outObj.__str_reper__ = to_string(out);
    }
    else
    {
        long out = bsMath<long>(stoi(headVar.__str_reper__), stoi(tailVar.__str_reper__), bsMathOper);
        outObj.__str_reper__ = to_string(out);
    }

    outObj.size = outObj.__str_reper__.size();
    return outObj;
}

bsMemoryObject bs_memory::stackPop( void )
{
    bsMemoryObject stackBack = this->bsStack.back();
    this->bsStack.pop_back();

    return stackBack;
}

bsMemoryObject bs_memory::getVar( const string& _ptrname )
{
    auto it = this->bsMemory.find(_ptrname);
    if (_ptrname.compare("null") == 0 || it == this->bsMemory.end())
        return this->bs_NULL();

    return it->second;
}

bsMemoryObject bs_memory::createObject( const string& _value)
{
    bsMemoryObject newObj;
    int varType = this->getType(_value);
    newObj.dType = varType;
    newObj.isConst = this->constFlag;
    newObj.size = _value.size();
    newObj.__str_reper__ = _value;
    this->constFlag = false;

    if (varType == BS_Num)
    {
        float val = stof(_value);
        int test = (int) val;
        if (val - test != 0)
            newObj.isFloat = true;
        
    }
    return newObj;
}