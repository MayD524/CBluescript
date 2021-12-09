
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
typedef const string cstr;
typedef const strref cstrref;

//inline string_vector comp_lines;
inline unsigned int curLineNo = 0;
inline unsigned int loopIDNum = 0;

struct token {
    bool   isCmd  = false;
    int    ivalue = 0;
    string svalue = "";
};

#define BS_FUNCTION_BLOCK 0
#define BS_IF_BLOCK       1
#define BS_ELIF_BLOCK     2
#define BS_ELSE_BLOCK     3
#define BS_WHILE_BLOCK    4


struct bs_variable {
	string name;
	bool isConst = false;
	bs_variable (const string& name, const bool& isConst) {
		this->name = name;
		this->isConst = isConst;
	}
	~bs_variable () {}
};

struct BS_block {
    string name;
    unsigned int type; // 0 = function, 1 = logic if, 2 = logic elif, 3 = logic else,4 = loops
    string cmp_str;
    string jmp_str;

    void block_end( string_vector& comp_lines )
    {
	//if (this->type == 49) return;
    if (this->type > 4) return;
	switch (this->type)
        {
            case BS_FUNCTION_BLOCK:
                comp_lines.push_back("ret");
                break;
            case BS_IF_BLOCK:
                comp_lines.push_back("label " + this->name);
                break;
            case BS_ELIF_BLOCK:
                comp_lines.push_back("label " + this->name);
                break;
            case BS_ELSE_BLOCK:
                comp_lines.push_back("label " + this->name);
                break;
            case BS_WHILE_BLOCK:
                comp_lines.push_back(cmp_str);
                comp_lines.push_back(jmp_str);
                //comp_lines.push_back("label " + this->name + "_endofloop");
                break;
            default:
		printf("Error: Unknown block type\nType:%u\nName:'%s'\n", this->type, this->name.c_str());
		throw("Error: Unknown block type");
                //exit(1);
        }
    }
};

typedef vector<token> token_vector;
typedef vector<BS_block> block_vector;
typedef vector<bs_variable> variable_vector;

#endif
