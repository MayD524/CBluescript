/*
    Author: May Draskovics
    Date: 9/30/2021
*/

#pragma once

#ifndef BS_TYPES_H
#define BS_TYPES_H

#include "UCPPL/UCPPL.h"
#include <iostream>
#include <sstream>
#include <bitset>
#include <string>
#include <vector>
#include <map>
#include <any>


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

struct numTypes
{
    float floatingRep = 0;
    long nonFloatingRep = 0;

    bool isFloat = false;

    bool checkIsFloat( string checkString )
    {
        float val = stof(checkString);
        int test = (int) val;
        if (val - test == 0)
            return false;
        return true;
    }

    void parseValue ( string inp )
    {
        if (this->checkIsFloat(inp))
        {
            this->isFloat = true;
            this->floatingRep = stof(inp);
            return;
        }
        this->nonFloatingRep = stol(inp);
        this->isFloat = false;
    }
};

class BS_GenericObject
{
    public:
        virtual Types getType() { return BS_Obj; }
        ~BS_GenericObject() = default; // default deconstruct
        string stringRepOfValue;
        size_t stringRepSize;

        // standard functions
        void setValue ( string value ) { }
        int __size__ ( void ) { return this->stringRepSize; }
        string __str__ ( void ) { return this->stringRepOfValue;}
};

class BS_BytesType: public BS_GenericObject
{

    public:
        vector<int> binArray;

        Types getType() { return BS_Bytes; }

        void setValue( string value )
        {
            this->stringRepOfValue = value;
            this->stringRepSize = value.size();
            for ( int i = 0; i < value.size(); i++)
            {
                if (value[i] == '1')
                    this->binArray.push_back(1);
                
                else
                    this->binArray.push_back(0);
            }
        }
        void printBinArray( void )
        {
            for (const int& i : this->binArray)
            {
                cout << i;
            }
            cout << endl;
        }

        int getBitAtIndex( int index )
        {
            if (index < this->binArray.size())
                return this->binArray[index];
            return 0;
        }

        int __size__ ( void ) { return this->stringRepSize; }
        string __str__( void ) { return this->stringRepOfValue; }

};

class BS_NullType: public BS_GenericObject
{
    public:
        Types getType() { return BS_NULL; }
        
        int __size__( void ) { return 0; }
        string __str__( void ) { return "NULL"; }
};

class BS_NumType: public BS_GenericObject
{

    public:
        numTypes curValue;
        Types getType() { return BS_Num; }

        bool isFloat ( void )
        {
            return this->curValue.isFloat;
        }
        
        template<typename T>
        T getValue ( void ) 
        { 
            if (this->isFloat())
                return this->curValue.floatingRep;
            
            return this->curValue.nonFloatingRep;
        }

        template<typename T>
        T operator~ ( void ) 
        {
            return this->getValue<T>();
        }

        void setValue( string value )
        {
            this->curValue.parseValue(value);
            // these two lines istg caused too much hassel :,)
            this->stringRepOfValue = value;
            this->stringRepSize = value.size();
        }
};

class BS_BoolType: public BS_GenericObject
{
    private:
        bool bitState;

    public:
        BS_BoolType()
        {
            this->bitState = false;
        }

        Types getType() { return BS_Bool; }
    
        bool getValue ( void )
        {
            return this->bitState;
        }

        bool operator! ( void )
        {
            this->bitState = !this->bitState;
            return this->bitState;
        }

        void setValue ( string value )
        {
            if (value.compare("true") == 0)
                this->bitState = true;
            else
                this->bitState = false;
        }

        string __str__ ( void )
        {
            if (this->bitState == false)
                return "false";
            else
                return "true";
        }
};

class BS_CharType: public BS_GenericObject
{
    private:
        string charToBits ( uchar chr )
        {
            return bitset<8>(chr).to_string();
        }

        string binRep;
        char repedChar;

    public:
        Types getType( void ) { return BS_Char; }
        void setValue( string value )
        {
            this->stringRepOfValue = value[0];
            this->stringRepSize = 1;
            this->repedChar = value[0];
            this->binRep = this->charToBits(this->repedChar);
        }

        char getChar ( void )
        {
            return repedChar;
        }
};

class BS_StringType: public BS_GenericObject
{
    public:
        Types getType( void ) { return BS_String; }

        vector<BS_CharType> characters;

        void setValue( string value )
        {
            this->stringRepOfValue = value;
            this->stringRepSize = value.size();

            for ( int i = 0; i < value.size(); i++ )
            {
                BS_CharType tmpChar = BS_CharType();
                tmpChar.setValue(string{value[i]});
                this->characters.push_back(tmpChar);
            }
        }

        BS_CharType getChar( int index )
        {
            if (index < characters.size())
                return characters[index];
            return BS_CharType();
        }
};

#endif