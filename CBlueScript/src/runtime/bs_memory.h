/*
    Author: May Draskovics
    Date: 10/12/2021
*/
#pragma once

#ifndef BS_MEMORY_H
#define BS_MEMORY_H

#include <iostream>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include "UCPPL/UCPPL.h"
#include "bs_defines.h"

using namespace std;

enum Types {
    BS_String,
    BS_Char,
    BS_Num,
    BS_Bool,
    BS_Bytes,
    BS_Array,
    BS_NULL,
    BS_Obj
};

struct bsMemoryObject
{
    string __str_reper__;
    int dType;
    size_t size = 0;
    bool isConst = false;
    bool isFloat = false;
};

struct ArrayLocs
{
    int start;
    int end;
};

class bs_memory
{
private:
    static bsMemoryObject bs_NULL( void );

public:
    bool constFlag = false;
    map<string, bsMemoryObject> bsMemory;
    map<string, long> labelLocations;
    map<string, ArrayLocs> bsArraySizes;

    vector<bsMemoryObject> arrayStack;
    vector<bsMemoryObject> bsStack;

    bs_memory() = default;
    ~bs_memory() = default;

    string stringReperOfType( Types type );
    void getAllKeys( void );
    void stackPush( const bsMemoryObject& _value );
    void eraseArray( const string& arrayName );
    void displayArray( void );
    void createArray( const string& arrayName, const int& arraySize);
    void arrayAppend( const string& arrayName, int index, const bsMemoryObject& obj );
    void varRemove( const string& _varptr );
    void putObjInMemory( const string& _ptrname, const bsMemoryObject& _data);
    void bsMovCmd( const string& _ptrname, const string& _value);

    int getType( const string& _value);
    int indexInArray( const string& arrayName, int index );
    int arraySize( const string& arrayName );
    bool varExists( const string& _ptrname );
    
    bsMemoryObject getFromArray( const string& arrayName, int index);
    bsMemoryObject runMath( const string& line, const int bsMathOper );
    bsMemoryObject stackPop( void );
    bsMemoryObject getVar( const string& _ptrname );
    bsMemoryObject createObject( const string& _value);
};


#endif