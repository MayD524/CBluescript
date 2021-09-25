/*
    Author: May Draskovics
    Date: 9/17/2021
*/
#pragma once

#ifndef BS_MEMORY_H
#define BS_MEMORY_H

#include <iostream>
#include <typeinfo>
#include <string.h>
#include <sstream>
#include <vector>
#include <string>
#include <map>

#include "UCPPL/UCPPL.h"

using namespace std;

struct bsMemoryObject {
    string value;
    string dType; // do type casting later
    bool isConst = false;
    int size = 0;
};

class bs_memory
{
private:
    bsMemoryObject nullInit( void )
    {
        bsMemoryObject nullVar;
        nullVar.value = "0";
        nullVar.dType = "NullType";
        nullVar.isConst = true;

        return nullVar;
    }

public:

    bool constFlag = false; // is declairing constant
    bool eqlFlag = false; // equal to flag
    bool grtFlag = false; // greater than flag

    bsMemoryObject bsNullVar = this->nullInit();
    map<string, bsMemoryObject> bsMemory;
    map<string, int> labelLocations;

    vector<bsMemoryObject> bsStack;

    bs_memory() { }

    string getType( string _value )
    {
        if (_value == "true" || _value == "false")
            return "bool";

        // numerics work as floats 
        else if (is_number(_value))
            return "numeric";

        else
            return "string";
    }

    void stackPush( bsMemoryObject _value )
    {
        this->bsStack.push_back(_value);
    }

    bsMemoryObject stackPop( void )
    {
        bsMemoryObject stackBack = this->bsStack.back();
        this->bsStack.pop_back();

        return stackBack;
    }

    bool varExists( string _ptrname )
    {
        auto it = this->bsMemory.find(_ptrname);

        if (it != this->bsMemory.end())
            return true;

        return false;
    }

    void varRemove( string varPtr )
    {
        if (this->varExists(varPtr))
        {
            this->bsMemory.erase(varPtr);
        }
    }

    void getAllKeys()
    {
        for(map<string,bsMemoryObject>::iterator iter = this->bsMemory.begin(); iter != this->bsMemory.end(); ++iter)
        {
            string key = iter->first;
            string value = iter->second.value;
            cout << key << ":" << value << endl;
        }
    }

    vector<float> mathParseToFloat( string _params )
    {
        vector<float> outPut;
        vector<string> arg_parsed = split(_params, ",");
        float head = 0;
        float tail = 0;
        if (this->varExists(arg_parsed[0]))
        {
            bsMemoryObject head_var = this->getVar(arg_parsed[0]);

            if (head_var.isConst == true)
            {
                debug("Cannot change constant type");
                return {0, 0};
            }
            // is an int or float type
            if (head_var.dType.compare("numeric") == 0)
            {
                head = stof(head_var.value);
                if (arg_parsed[1].rfind("%", 0) == 0) 
                {
                    bsMemoryObject trail_var = this->getVar(split(arg_parsed[1], "%")[1]);
                    if (trail_var.dType.compare("numeric") == 0)
                    {
                        tail = stof(trail_var.value);
                    }
                    else
                        debug("Invaild trailvar type");
                }
                else
                {
                    tail = stof(arg_parsed[1]);
                }
            }
            else
                debug("Invalid headvar type");

        }
        else
            debug("First var in add wasn't correct");

        return {head, tail};
    }

    bsMemoryObject getVar( string _ptrname )
    {
        if (_ptrname.compare("null") == 0)
            return this->bsNullVar;

        auto it = this->bsMemory.find(_ptrname);
        if (it != this->bsMemory.end())
            return it->second;
        return this->bsNullVar;
    }

    bsMemoryObject createObject( string _value )
    {
        // create a new object
        bsMemoryObject newObj;
        newObj.value = _value;
        newObj.dType = getType(_value);

        if (newObj.dType.compare("numeric") == 0)
            newObj.value = to_string(stof(_value));

        newObj.isConst = this->constFlag;
        newObj.size = _value.size();

        this->constFlag = false; // set to false

        return newObj;
    }

    void putObjInMemory( string _ptrname, bsMemoryObject _data)
    {
        if (this->varExists(_ptrname))
        {
            bool constFlag = this->getVar(_ptrname).isConst;
            if (constFlag == true)
            {
                debug("Cannot change an immutable variable");
                return;
            }
            else
            {
                this->bsMemory.erase(_ptrname);
            }
        }

        this->bsMemory.insert({ _ptrname, _data });
        return;
    }

    void bsMovCmd(string _ptrName, string _value)
    { 
        if (_value.rfind("%", 0) == 0)
            _value = getVar(split(_value, "%")[1]).value;

        // parse all types later
        bsMemoryObject memObj = this->createObject(_value);

        // add object to memory
        this->putObjInMemory(_ptrName, memObj);
    }
};


#endif