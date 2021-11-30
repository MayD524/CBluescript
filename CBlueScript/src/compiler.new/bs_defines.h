
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
    int type; // 0 = function, 1 = logic if, 2 = logic elif, 3 = logic else,4 = loops

    BS_block(const string& name, const int& type)
    {
        this->name = name;
        this->type = type;
    }
    ~BS_block() {}

    void block_end( string_vector& comp_lines )
    {
        switch (type)
        {
            case 0:
                comp_lines.push_back("ret");
                break;
            case 1:
                comp_lines.push_back("label " + name + "_endofif");
                break;
            case 2:
                comp_lines.push_back("label " + name + "_endofelif");
                break;
            case 3:
                comp_lines.push_back("label " + name + "_endofelse");
                break;
            case 4:
                comp_lines.push_back("label " + name + "_endofloop");
                break;
            default:
                cerr << "Error: Unknown block type" << endl;
                exit(1);
        }
    }
};

typedef vector<token> token_vector;
typedef vector<BS_block> block_vector;
typedef vector<bs_variable> variable_vector;

#endif
