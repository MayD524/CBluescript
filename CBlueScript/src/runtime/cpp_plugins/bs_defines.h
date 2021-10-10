
#pragma once

#ifndef BS_DEFINES_H
#define BS_DEFINES_H

#include "bs_types.h"

using namespace std;

enum BS_OPERATIONS
{
    BS_ADD,
    BS_SUB,
    BS_DIV,
    BS_MUL
};

template<typename T>
T bsMath(BS_NumType num1, BS_NumType num2, const int bsMathOper)
{
    T number1 = num1.getValue<T>();
    T number2 = num2.getValue<T>();
    switch ( bsMathOper )
    {
        case BS_ADD:
            return number1 + number2;
        
        case BS_SUB:
            return number1 - number2;

        case BS_DIV:
            return number1 / number2;
        
        case BS_MUL:
            return number1 * number2;
    }

    return 0;
}



#endif