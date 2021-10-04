/*
    author: May Draskovics
    date: 10/2/2021
*/
#pragma once

#ifndef BS_MEMORY_NEW_H
#define BS_MEMORY_NEW_H

#include <iostream>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <any>

#include "UCPPL/UCPPL.h"
#include "bs_defines.h"
#include "bs_types.h"

struct bsMemoryObject
{
    bool isConst = false;
    int isObjectOf;
    any obj;
};


class bs_memory
{
    private:
        const BS_NullType NULLOBJECT = BS_NullType();

        bsMemoryObject nullObj( void )
        {
            bsMemoryObject nll;
            nll.isConst = true;
            nll.isObjectOf = BS_NULL;
            nll.obj = NULLOBJECT;

            return nll;
        }

    public:
        bool constFlag = false;

        map<string, bsMemoryObject> bsMemory;
        map<string, long> labelLocations;

        vector<bsMemoryObject> bsStack;

        bs_memory() = default;
        ~bs_memory() = default;

        int getType( string _value )
        {
            if (_value.compare("true") == 0 || _value.compare("false") == 0)
                return BS_Bool;

            else if (is_number(_value))
                return BS_Num;
            
            else if (_value == "NULL")
                return BS_NULL;
            
            else
                return BS_String;
        }

        void getAllKeys( void )
        {
            for(map<string,bsMemoryObject>::iterator iter = this->bsMemory.begin(); iter != this->bsMemory.end(); ++iter)
            {
                string key = iter->first;
                string value = this->getStringReperOfVariable(iter->second.isObjectOf, iter->second.obj);
                cout << key << ":" << value << endl;
            }
        }

        string stringReperOfType( int varType )
        {
            switch( varType )
            {
                case BS_NULL  : return "NULL";
                case BS_String: return "string";
                case BS_Num   : return "numeric";
                case BS_Bool  : return "bool";
                case BS_Char  : return "char";
                default       : return "generic";
            }
        }

        void stackPush( const bsMemoryObject& _value )
        {
            this->bsStack.push_back(_value);
        }

        template<typename T>
        T returnAsType ( any _value )
        {
            return any_cast<T>(_value);
        }

        bsMemoryObject runMath( const string& line, const int bsMathOper )
        {
            bsMemoryObject outObj;

            // these two aren't required?
            bsMemoryObject headVar;


            BS_NumType outVar = BS_NumType();

            BS_NumType head = BS_NumType(); // the head var
            BS_NumType tail = BS_NumType(); // the tail var
            outObj.isObjectOf = outVar.getType();

            vector<string> args = split(line, ",");

            if(this->varExists(args[0]))
            {
                headVar = this->getVar(args[0]);

                // kill if is constant (illegal operation)
                if (headVar.isConst == true)
                {
                    cout << "Cannot change constant type" << endl;
                    exit(1);
                }

                head = this->returnAsType<BS_NumType>(headVar.obj);

                if (args[1].rfind("%", 0) == 0)
                {
                    bsMemoryObject tailVar = this->getVar(split(args[1], "%")[1]);
                    tail = this->returnAsType<BS_NumType>(tailVar.obj);
                }
                else
                    tail.setValue(args[1]);

                if (head.isFloat() || tail.isFloat())
                {

                    float mathReturn = bsMath<float>(head, tail, bsMathOper);
                    outVar.setValue(to_string(mathReturn));
                }
                else
                {
                    long mathReturn = bsMath<long>(head, tail, bsMathOper);
                    outVar.setValue(to_string(mathReturn));
                }

                outObj.isConst = this->constFlag;
                outObj.isObjectOf = BS_Num;
                outObj.obj = outVar;
                
                
            }
            else
            {
                cout << "Cannot do operations on non-variables" << endl;
                exit(1);
            }

            return outObj;
        }

        string getStringReperOfVariable( const int& typeID, const any& obj )
        {
            switch( typeID )
            {
                case BS_String: 
                {
                    return this->returnAsType<BS_StringType>(obj).__str__();
                }
                case BS_Bool:
                {
                    return this->returnAsType<BS_BoolType>(obj).__str__();
                }
                case BS_Num:
                {
                    return this->returnAsType<BS_NumType>(obj).__str__();
                }
                default:
                {
                    return "NULL";
                }
            }
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
        
        void varRemove( string _varptr)
        {
            if (this->varExists(_varptr))
                this->bsMemory.erase(_varptr);
        }

        bsMemoryObject getVar( string _ptrname )
        {
            if (_ptrname.compare("null") == 0)
                return this->nullObj();
            
            auto it = this->bsMemory.find(_ptrname);
            if (it != this->bsMemory.end())
                return it->second;
            return this->nullObj();
        }

        bsMemoryObject createObject( string _value )
        {
            bsMemoryObject newObj;
            int varType = this->getType(_value);
            switch (varType)
            {
                case BS_Num:
                {
                    BS_NumType numVar = BS_NumType();
                    numVar.setValue( _value );
                    newObj.isConst = this->constFlag;
                    newObj.isObjectOf = BS_Num;
                    newObj.obj = numVar;
                    this->constFlag = false;
                    return newObj;
                }
                case BS_Bool:
                {
                    BS_BoolType boolVar = BS_BoolType();
                    boolVar.setValue( _value );
                    newObj.isConst = this->constFlag;
                    newObj.isObjectOf = BS_Bool;
                    newObj.obj = boolVar;
                    this->constFlag = false;
                    return newObj;
                }
                case BS_String:
                {
                    BS_StringType stringVar = BS_StringType();
                    stringVar.setValue( _value );
                    newObj.isConst = this->constFlag;
                    newObj.isObjectOf = BS_String;
                    newObj.obj = stringVar;
                    this->constFlag = false;
                    return newObj;
                }
                default:
                {
                    return this->nullObj();
                }
            }
        }

    void putObjInMemory( string _ptrname, bsMemoryObject _data )
    {
        if (this->varExists(_ptrname))
        {
            if ( this->getVar(_ptrname).isConst == true)
            {
                cout << "Cannot change an immutable variable" << endl;
                exit(1);
            }
            else
                this->bsMemory.erase(_ptrname);
        }

        this->bsMemory.insert({ _ptrname, _data });
        return;
    }

    void bsMovCmd( string _ptrname, string _value )
    {
        bsMemoryObject memObject;
        if (_value.rfind("%", 0) == 0)
            memObject = this->getVar(split(_value, "%")[1]);
        else
            memObject = this->createObject(_value);
        
        this->putObjInMemory( _ptrname, memObject );
    }

};



#endif