
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
struct token {
    bool   isCmd  = false;
    int    ivalue = 0;
    string svalue = "";
};

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

    void block_end( string_vector& comp_lines )
    {
	//if (this->type == 49) return;
    if (this->type > 4) return;
	switch (this->type)
        {
            case 0:
                comp_lines.push_back("ret");
                break;
            case 1:
                comp_lines.push_back("label " + this->name + "_endofif");
                break;
            case 2:
                comp_lines.push_back("label " + this->name + "_endofelif");
                break;
            case 3:
                comp_lines.push_back("label " + this->name + "_endofelse");
                break;
            case 4:
                comp_lines.push_back("label " + this->name + "_endofloop");
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
