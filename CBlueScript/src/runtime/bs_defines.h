/*
    Author: May Draskovics
    Date: 10/14/2021

    Description:
        Hold generic defines used throughout the
        language

*/
#pragma once

#ifndef BS_DEFINES_H
#define BS_DEFINES_H

#define BLUESCRIPT_RAND_DEFAULTMIN 0
#define BLUESCRIPT_RAND_DEFAULTMAX 9999999

using namespace std;

enum BS_OPERATIONS
{
    BS_ADD,
    BS_SUB,
    BS_DIV,
    BS_MUL
};

#endif