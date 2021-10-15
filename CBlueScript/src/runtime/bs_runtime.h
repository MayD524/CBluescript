/*
    Author: May Draskovics
    Date: 9/17/2021
*/

#pragma once
#ifndef BS_RUNTIME_H
#define BS_RUNTIME_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

#include "UCPPL/UCPPL.h"

#include "bs_memory.h"
#include "bs_arrayTools.h"
#include "bs_defines.h"
#include "bs_funcs.h"

using namespace std;

class bs_runtime
{
private:

    // we don't want this to be negative
    // allows for 2^64 lines (hopefully more then enough)
    map<string, void(*)(bs_memory* mem, const string& args)> linkedFuncs;
    unsigned long program_counter = 0;
    unsigned long max_program_lines = 0;
    vector<unsigned long> func_returns;
    unsigned long function_return = 0;
    vector<string> program_lines;
    bs_memory memoryObject;
    bool debugEnabled = false;
    bool autoWalkEnabled = false;
    bool canRun = true;
    bool eqlFlag = false;
    bool grtFlag = false;

    // just convert a string to a sum of all chars in it
    int dumbHash( string str )
    {
        const char* cStr = str.c_str();
        int dHash = 0;

        for (int i = 0; i < str.size(); i++)
            dHash += (uchar) cStr[i];

        return dHash;
    }

    void run_jmp( string label )
    {
        if (label.compare("EOF_BS_JMP_POINT") == 0)
        {
            exit(0);
            this->canRun = false;
            return;
        }
        this->eqlFlag = false;
        this->grtFlag = false;

        auto it = this->memoryObject.labelLocations.find(label);
        if (it != this->memoryObject.labelLocations.end())
            this->program_counter = it->second;

        else
        {
            printf("Label %s does not exist (line : %li)", label.c_str(), this->program_counter);
            this->canRun = false;
        }

    }


public:

    bs_runtime(vector<string> lines, bool debugEnabled, map<string, void(*)(bs_memory* mem, const string& args)> loadedFuncs, bool autoWalk)
    {
        this->program_lines = lines;
        this->max_program_lines = lines.size();
        this->debugEnabled = debugEnabled;
        this->autoWalkEnabled = autoWalk;
        this->linkedFuncs = loadedFuncs;
    }

    ~bs_runtime ( void ) = default;

    int run_cmd( int cmd, string cmd_args )
    {
        if (this->debugEnabled)
        {
            cout << "\n\n\nMemory:\n";
            this->memoryObject.getAllKeys();
            cout << "\n\nStack:\n";
            for (bsMemoryObject obj : this->memoryObject.bsStack)
            {
                cout << obj.__str_reper__ << endl;
            }
            cout << "\n\nArray Stack:\n";
            this->memoryObject.displayArray();
            cout << "\n\nFlags:\n";
            cout << "Eql Flag:" << this->eqlFlag << endl;
            cout << "Grt Flag:" << this->grtFlag << endl;
        }
        //debug(cmd);
        switch (cmd)
        {
            case 338: // mov
            {
                bs_MOV(this->memoryObject, cmd_args);
                break;
            }

            case 445: // rint min,Max
            {
                bs_RINT(this->memoryObject, cmd_args);
                break;
            }

            case 418: // rem
            {
                bs_REM(this->memoryObject, cmd_args);
                break;
            }

            case 450: // type <return ptr>,<var pointer>
            {
                bs_GETTYPE(this->memoryObject, cmd_args);
                break;
            }

            case 536: // index <return ptr>,<var ptr>[<index>]
            {
                bs_INDEX(this->memoryObject, cmd_args);
                break;
            }

            case 332: // set <flag>,(1|0)
            {
                bs_SET(this->memoryObject, cmd_args);
                break;
            }

            case 656: // sizeof <return ptr>,<var ptr>
            {
                bs_SIZEOF(this->memoryObject, cmd_args);
                break;
            }

            case 923: // read_file <var_ptr>,filename
            {
                bs_READ(this->memoryObject, cmd_args);
                break;
            }
            
            case 1066: // write_file filename,<array>
            {
                bs_WRITE(this->memoryObject, cmd_args);
                break;
            }

            case 1081: // array_size <return pointer>,<array name>
            {
                bs_ARRAY_SIZE(this->memoryObject, cmd_args);
                break;
            }

            case 1166: // array_erase <array name>
            {
                bs_ARRAY_ERASE(this->memoryObject, cmd_args);
                break;
            }

            case 1052: // array_make <array name>,<array size>
            {
                bs_ARRAY_MAKE(this->memoryObject, cmd_args);
                break;
            }

            case 958: // array_get <outVar>,<array>[<index>]
            {
                bs_ARRAY_GET(this->memoryObject, cmd_args);
                break;
            }

            case 970: // array_set
            {
                bs_ARRAY_SET(this->memoryObject, cmd_args);
                break;
            }

            case 344: // out (print)
            {
                string printString;
                if (cmd_args.rfind("%", 0) == 0)
                {
                    bsMemoryObject mem = this->memoryObject.getVar(split(cmd_args, "%")[1]);
                    printString =  mem.__str_reper__;
                }
                else
                    printString = cmd_args;
                replace(printString, "\\n", "\n");

                cout << printString;

                break;
            }

            case 215: // in (input)
            {
                vector<string> arg_split = split(cmd_args, ",", true);
                string inputValue;

                if (arg_split[1].rfind("%", 0) == 0)
                {
                    bsMemoryObject tmpObj = this->memoryObject.getVar(split(arg_split[1], "%")[1]);
                    arg_split[1] = tmpObj.__str_reper__;
                }
                replace(arg_split[1], "\\n", "\n");
                // print prompt
                cout << arg_split[1];
                getline(cin, inputValue);

                this->memoryObject.bsMovCmd(arg_split[0], inputValue);
                break;
            }

            // control flow & lables
            case 320: // cmp
            {
                vector<string> cmdSplit = split(cmd_args, ",", true);
                bsMemoryObject headVar;
                bsMemoryObject tailVar;

                // check if a var is a var else make it an object
                if (cmdSplit[0].rfind("%", 0) == 0)
                    headVar = this->memoryObject.getVar(split(cmdSplit[0], "%")[1]);

                else
                    headVar = this->memoryObject.createObject(cmdSplit[0]);

                if (cmdSplit[1].rfind("%", 0) == 0)
                    tailVar = this->memoryObject.getVar(split(cmdSplit[1], "%")[1]);

                else
                    tailVar = this->memoryObject.createObject(cmdSplit[1]);
                    
                if (headVar.__str_reper__.compare(tailVar.__str_reper__) == 0)
                    this->eqlFlag = true;

                else
                    this->eqlFlag = false;
                
                if (headVar.dType == BS_Num && tailVar.dType == BS_Num)
                {

                    if (headVar.isFloat || tailVar.isFloat)
                    {
                        if (stof(headVar.__str_reper__) > stof(tailVar.__str_reper__))
                            this->grtFlag = true;
                        else
                            this->grtFlag = false;;
                    }
                    else
                    {
                        if (stol(headVar.__str_reper__) > stol(tailVar.__str_reper__))
                            this->grtFlag = true;
                        else
                            this->grtFlag = false;;
                    }
                }
                

                break;
            }

            case 310: // jge
            {
                if (this->eqlFlag == true || this->grtFlag == true)
                {
                    this->run_jmp(cmd_args);
                }
                break;
            }

            case 319: // jng
            {
                if (this->grtFlag == false)
                {
                    this->run_jmp(cmd_args);
                }
                break;
            }

            case 209: // jg
            {
                if (this->grtFlag == true)
                {
                    this->run_jmp(cmd_args);
                }
                break;
            }

            case 317: // jne
            {
                if (this->eqlFlag == false)
                {
                    this->run_jmp(cmd_args);
                }
                break;
            }

            case 207: // je
            {
                if (this->eqlFlag == true)
                {
                    this->run_jmp(cmd_args);
                }
                break;
            }

            case 327: // jmp
            {
                this->run_jmp(cmd_args);
                break;
            }

            // Stack related items
            case 448: // push   
            {
                bsMemoryObject objToPush;
                if (cmd_args.rfind("%", 0) == 0)
                    objToPush = this->memoryObject.getVar(split(cmd_args, "%")[1]);
                else
                    objToPush = this->memoryObject.createObject(cmd_args);

                this->memoryObject.stackPush(objToPush);
                break;
            }

            case 335: // pop
            {
                bsMemoryObject popedObject = this->memoryObject.stackPop();
                this->memoryObject.varRemove(cmd_args);
                this->memoryObject.putObjInMemory(cmd_args, popedObject);
                break;
            }

            case 421: // peek outvar
            {
                // works kinda like pop but does
                // not remove it from the stack
                bsMemoryObject peekObject = this->memoryObject.bsStack.back();
                this->memoryObject.putObjInMemory(cmd_args, peekObject);
                break;
            }

            // math stuff
            // this might be able to be reduced but leaving it for now
            case 297: // add
            {
                vector<string> splitArgs = split(cmd_args, ",", true);
                string varName = splitArgs[0];

                bsMemoryObject headVar = this->memoryObject.getVar(varName);
                bsMemoryObject tailVar;

                if (headVar.dType == BS_String)
                {
                    if (splitArgs[1].rfind("%", 0) == 0)
                        tailVar = this->memoryObject.getVar(split(splitArgs[1], "%", true)[1]);
                    else
                        tailVar = this->memoryObject.createObject(splitArgs[1]);

                    this->memoryObject.bsMovCmd(varName, tailVar.__str_reper__);
                }
                else
                {
                    bsMemoryObject parsedMath = this->memoryObject.runMath(cmd_args, BS_ADD);
                    this->memoryObject.putObjInMemory(varName,parsedMath);
                }
                break;
            }

            case 330: // sub
            {
                string varName = split(cmd_args, ",", true)[0];
                bsMemoryObject parsedMath = this->memoryObject.runMath(cmd_args, BS_SUB);
                this->memoryObject.putObjInMemory(varName,parsedMath);
                break;
            }

            case 334: // mul
            {
                string varName = split(cmd_args, ",", true)[0];
                bsMemoryObject parsedMath = this->memoryObject.runMath(cmd_args, BS_MUL);
                this->memoryObject.putObjInMemory(varName,parsedMath);
                break;
            }

            case 323: // div
            {
                string varName = split(cmd_args, ",", true)[0];
                bsMemoryObject parsedMath = this->memoryObject.runMath(cmd_args, BS_DIV);
                this->memoryObject.putObjInMemory(varName,parsedMath);
                break;
            }
        
            case 441: // goto
            {
                if (is_number(cmd_args))
                {
                    unsigned long newLineNo = stof(cmd_args);
                    if (newLineNo < this->program_counter)
                        this->program_counter = newLineNo;
                    else
                    {
                        printf("goto on line <%li> out of range", this->program_counter);
                        this->canRun = false;
                        return 1;
                    }
                }
                break;
            }
            

            case 412: // call
            {
                auto it = this->memoryObject.labelLocations.find(cmd_args);

                if (it != this->memoryObject.labelLocations.end())
                {
                    this->func_returns.push_back(this->program_counter);
                    //this->function_return = this->program_counter;
                    this->program_counter = it->second;
                }
                else
                {
                    printf("function %s does not exist (line: %li)", cmd_args.c_str(), this->program_counter);
                    this->canRun = false;
                    return 1;
                }
                break;
            }

            case 331: // ret
            {
                this->program_counter = this->func_returns.back();
                this->func_returns.pop_back();
                this->function_return = 0;
      
                break;
            }
            default:
            {
                return 999;
            }
        }
        return 0; // it worked without error
    }

    // get label locations
    void preRead( void )
    {
        for (int lineNo = 0; lineNo < this->program_lines.size(); lineNo++)
        {
            
            // kill comments
            this->program_lines[lineNo] = trim(split(this->program_lines[lineNo], ";")[0]);
            if ( this->program_lines[lineNo].rfind("label",0) == 0)
            {
                string label_ptr = split(this->program_lines[lineNo], " ")[1];
                this->memoryObject.labelLocations.insert({ label_ptr, lineNo });
            }
        }
    }
    
    int handle_plugins( const string& cmd_line )
    {
        vector<string> splitCmd = split(cmd_line, " ", true);
        auto it = this->linkedFuncs.find(splitCmd[0]);

        if (it == this->linkedFuncs.end())
        {
            return 999;
        }
        it->second(&this->memoryObject, splitCmd[1]);
        return 0;
    }

    void runtimeMain( void )
    {
        int cmd_out = 0;
        this->preRead();
        while (this->program_counter < this->max_program_lines && this->canRun == true)
        {
            string current_line = this->program_lines[this->program_counter];
            if (this->debugEnabled == true)
            {
                string dummy;
                //this->memoryObject.getAllKeys();
                cout << "Line Number: " << this->program_counter << endl;
                cout << "Line Data: " << current_line << endl;
                if (this->program_counter + 1 < this->max_program_lines)
                    cout << "Next: " << this->program_counter + 1 << ":" << this->program_lines[this->program_counter+1];
                cout << current_line << endl;
                if (this->autoWalkEnabled)
                    getline(cin, dummy);
            }

            vector<string> lineSplit = split(current_line, " ", true);
            if( lineSplit.size() == 1)
                lineSplit.push_back(" ");
            cmd_out = handle_plugins(current_line);

            if (cmd_out == 999)
                cmd_out = run_cmd(dumbHash(lineSplit[0]), lineSplit[1]);

            else if (cmd_out != 0)
            {
                cout << "There was an issue\n";
                cout << cmd_out << endl;
                this->canRun = false;
                return;
            }

            this->program_counter++;
        }
    }

};

#endif
