/*
    Author: May Draskovics
    Date: 9/20/2021
*/

#pragma once

#ifndef BS_PARSER_H
#define BS_PARSER_H

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <map>

using namespace std;

struct bsMath
{
    bool hasMath = false;
    string mathOper = "";
};

class bs_parser
{
private:
    unsigned long program_lines = 0;    // total lines we have
    unsigned long program_conunter = 0; // for loop

    map<string, string> declairedVars;  // declaired vars and their asm counterpart

    vector<string> parsedLines;         // what we have parsed
    vector<string> fileLines;           // pre-parsed lines

    vector<string> keywords = {          // all default key words
        "if",
        "else",
        "function",
        "const"
    };

    vector<string> mathOpers = {
        "*", "/", "+", "-"
    };

    string homeFile;                    // the core file
    string currentFile;                 // current file

    bool canRun = true;                 // bool for error checking (false if errored (killed execution))

    bool startsWith( const string str, const string substr )
    {
        if ( str.rfind(substr, 0) == 0 )
            return true;
        
        return false;
    }

    bsMath containsAny( const string str, vector<string> subStrs )
    {
        bsMath mobj; 
        for (const string subStr : subStrs)
        {
            if ( str.find(subStr) != string::npos )
            {
                mobj.hasMath = true;
                mobj.mathOper = subStr;
                return mobj;
            }

        }
        return mobj;
    }

public:

    // class constructor
    bs_parser( vector<string> file_lines, string workingFile )
    {
        /*
            file_lines (vector<string>) -> The unparsed lines from the core file
            workingFile (string) -> the file we are starting work in.
        */

       this->fileLines = file_lines;
       this->program_lines = file_lines.size();
       this->homeFile = workingFile;
    }

    void movMakeLine( string varName, string baseValue )
    {

    }

    void parseKeyWord( string keyline )
    {

    }

    void parseNonKeyWord( string nkeyline )
    {
        if (nkeyline.find("=") != string::npos)
        {
            // is an expression
            vector<string> parsedLine = split(nkeyline, "=");
            bsMath hasMath = this->containsAny(parsedLine[0], this->mathOpers);

            string newLine = "";
            string varToAssign;
            string varExpresVal;
            
            if (hasMath.hasMath == true)
            {
                // varname is included in an expression
                varToAssign = trim(split(parsedLine[0], hasMath.mathOper)[0]);
                varExpresVal = parsedLine[0] + trim(parsedLine[1]);
            }
            else
            {
                // var name is just a delic
                varToAssign = trim(parsedLine[0]);
                varExpresVal = trim(parsedLine[0]);
                cmd += varToAssign;
                bsMath ctMath = this->containsAny(varExpresVal, mathOpers);

                if (ctMath.hasMath == true)
                {
                    // do the math stuff
                }
                else
                {
                    // just a var delic
                    
                }
            }
            
        }
        else if (nkeyline.find("(") != string::npos)
        {
            // is a func call
        }


    }

    void lineToken( string line )
    {
        for (const string& kw : keywords )
        {
            if ( this->startsWith(line, kw) == true )
            {
                this->parseKeyWord( line );
                return;
            }
        }
        this->parseNonKeyWord( line );
        return;
    }

    void parseFile( void )
    {
        while (this->program_conunter < this->program_lines && this->canRun == true)
        {
            // ignore comments
            this->fileLines[this->program_conunter] = split(this->fileLines[this->program_conunter], "//")[0];
            this->lineToken(this->fileLines[this->program_conunter]);
            this->program_conunter++;
        }
    }
};



#endif