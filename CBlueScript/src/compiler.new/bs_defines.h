
#pragma once
#ifndef BS_DEFINES_H
#define BS_DEFINES_H


#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef vector<string> string_vector;
typedef vector<int> int_vector;
typedef string& strref;
typedef const strref cstrref;

struct token {
    bool   isCmd  = false;
    int    ivalue = 0;
    string svalue = "";
};

typedef vector<token> token_vector;



#endif