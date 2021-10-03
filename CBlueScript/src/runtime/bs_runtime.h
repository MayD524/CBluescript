/*
    Author: May Draskovics
    Date: 9/17/2021
*/

#pragma once
#ifndef BS_RUNTIME_H
#define BS_RUNTIME_H

#include <iostream>
#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"
#include "bs_memory_new.h"
#include "bs_defines.h"

using namespace std;

class bs_runtime
{
private:

    // we don't want this to be negative
    // allows for 2^64 lines (hopefully more then enough)
    unsigned long program_counter = 0;
    unsigned long max_program_lines = 0;
    vector<unsigned long> func_returns;
    unsigned long function_return = 0;
    vector<string> program_lines;
    bs_memory memoryObject;
    bool canRun = true;

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
        auto it = this->memoryObject.labelLocations.find(label);

        if (it != this->memoryObject.labelLocations.end())
            this->program_counter = it->second;

        else
        {
            printf("Label %s does not exist (line : %li)", label.c_str(), this->program_counter);
            this->canRun = false;
            return;
        }

        this->memoryObject.eqlFlag = false;
        this->memoryObject.grtFlag = false;
    }


public:
    bs_runtime(vector<string> lines)
    {
        this->program_lines = lines;
        this->max_program_lines = lines.size();
    }

    int run_cmd( int cmd, string cmd_args )
    {
        #ifdef DEBUG
            cout << "\n\n\nMemory:\n";
            this->memoryObject.getAllKeys();
            cout << "\n\nStack:\n";
            for (bsMemoryObject obj : this->memoryObject.bsStack)
            {
                cout << this->getStringReperOfVariable(obj.isObjectOf, obj.obj) << endl;
            }
            cout << "\n\nFlags:\n";
            cout << "Eql Flag:" << this->memoryObject.eqlFlag << endl;
            cout << "Grt Flag:" << this->memoryObject.grtFlag << endl;
        #endif
        //debug(cmd);
        switch (cmd)
        {
            case 338: // mov
            {
                vector<string> arg_parse = split(cmd_args, ",");
                this->memoryObject.bsMovCmd(arg_parse[0], arg_parse[1]);
                break;
            }

            case 418: // rem
            {
                if (cmd_args.rfind("%", 0) == 0)
                    this->memoryObject.varRemove(split(cmd_args, "%")[1]);
                
                else
                {
                    cout << "variable " << cmd_args << " does not exist.\n";
                    return 1;
                }
                break;
            }

            case 450: // type <return ptr>,<var pointer>
            {
                vector<string> argSplit = split(cmd_args, ",");
                if (argSplit[1].rfind("%",0) == 0)
                {
                    bsMemoryObject obj = this->memoryObject.getVar(split(cmd_args, "%")[1]);
                    this->memoryObject.bsMovCmd(argSplit[0], this->memoryObject.stringReperOfType(obj.isObjectOf));
                }
                else
                {
                    cout << "Variable " << argSplit[1] << " does not exist or hasn't been defined.\n";
                    return 1;
                }
                break;
            }

            case 427: // cast
            {
                break;
            }

            case 656: // sizeof <return ptr>,<var ptr>
            {
                break;
            }

            case 344: // out (print)
            {
                if (cmd_args.rfind("%", 0) == 0)
                {
                    bsMemoryObject mem = this->memoryObject.getVar(split(cmd_args, "%")[1]);
                    cout << this->memoryObject.getStringReperOfVariable(mem.isObjectOf, mem.obj) << endl;
                }
                else
                    cout << cmd_args << endl;
                break;
            }

            case 215: // in (input)
            {
                vector<string> arg_split = split(cmd_args, ",");
                string inputValue;

                if (arg_split[1].rfind("%", 0) == 0)
                {
                    bsMemoryObject tmpObj = this->memoryObject.getVar(split(arg_split[1], "%")[1]);
                    arg_split[1] = this->memoryObject.getStringReperOfVariable(tmpObj.isObjectOf, tmpObj.obj);
                }
                // print prompt
                cout << arg_split[1];
                getline(cin, inputValue);

                this->memoryObject.bsMovCmd(arg_split[0], inputValue);
                break;
            }

            // control flow & lables
            case 320: // cmp
            {
                vector<string> cmdSplit = split(cmd_args, ",");
                bsMemoryObject headVar;
                bsMemoryObject tailVar;

                // check if a var is a var else make it an object
                if (cmdSplit[0].rfind("%", 0) == 0)
                    headVar = this->memoryObject.getVar(split(cmdSplit[0], "%")[1]);

                else
                    headVar = this->memoryObject.createObject(split(cmdSplit[0], "%")[1]);

                if (cmdSplit[1].rfind("%", 0) == 0)
                    tailVar = this->memoryObject.getVar(split(cmdSplit[1], "%")[1]);

                else
                    tailVar = this->memoryObject.createObject(split(cmdSplit[1], "%")[1]);

                if (this->memoryObject.getStringReperOfVariable(headVar.isObjectOf, headVar.obj).compare(this->memoryObject.getStringReperOfVariable(headVar.isObjectOf, tailVar.obj)))
                    this->memoryObject.eqlFlag = true;

                else
                {
                    if (headVar.isObjectOf == BS_Num && tailVar.isObjectOf == BS_Num)
                    {
                        BS_NumType headVarNum = any_cast<BS_NumType>(headVar.obj);
                        BS_NumType tailVarNum = any_cast<BS_NumType>(tailVar.obj);
                        if (headVarNum.isFloat() || tailVarNum.isFloat())
                        {
                            if(headVarNum.getValue<float>() > tailVarNum.getValue<float>())
                                this->memoryObject.grtFlag = true;
                        }
                        else
                        {
                            if(headVarNum.getValue<long>() > tailVarNum.getValue<long>())
                                this->memoryObject.grtFlag = true;
                        }
                    }
                }

                break;
            }

            case 310: // jge
            {
                if (this->memoryObject.eqlFlag == true || this->memoryObject.grtFlag == true)
                {
                    run_jmp(cmd_args);
                }
                break;
            }

            case 319: // jng
            {
                if (this->memoryObject.grtFlag == false)
                {
                    run_jmp(cmd_args);
                }
                break;
            }

            case 209: // jg
            {
                if (this->memoryObject.grtFlag == true)
                {
                    run_jmp(cmd_args);
                }
                break;
            }

            case 317: // jne
            {
                if (this->memoryObject.eqlFlag == false)
                {
                    run_jmp(cmd_args);
                }
                break;
            }

            case 207: // je
            {
                if (this->memoryObject.eqlFlag == true)
                {
                    run_jmp(cmd_args);
                }
                break;
            }

            case 327: // jmp
            {
                run_jmp(cmd_args);
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
                this->memoryObject.putObjInMemory(cmd_args, popedObject);
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

                if (headVar.isObjectOf == BS_String)
                {
                    if (splitArgs[1].rfind("%", 0) == 0)
                        tailVar = this->memoryObject.getVar(split(splitArgs[1], "%", true)[1]);
                    else
                        tailVar = this->memoryObject.createObject(splitArgs[1]);

                    this->memoryObject.bsMovCmd(varName, this->memoryObject.getStringReperOfVariable(headVar.isObjectOf, headVar.obj) + this->memoryObject.getStringReperOfVariable(tailVar.isObjectOf, tailVar.obj));
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
    
    void runtimeMain( void )
    {
        this->preRead();
        while (this->program_counter < this->max_program_lines && this->canRun == true)
        {
            string current_line = this->program_lines[this->program_counter];
            #ifdef BLUE_SCRIPT_STEPTHROUGH
                string dummy;
                //this->memoryObject.getAllKeys();
                cout << "Line Number: " << this->program_counter << endl;
                cout << "Line Data: " << current_line << endl;
                if (this->program_counter + 1 < this->max_program_lines)
                    cout << "Next: " << this->program_counter + 1 << ":" << this->program_lines[this->program_counter+1];
                cout << current_line << endl;
                #ifndef BS_AUTOWALK
                    getline(cin, dummy);
                #endif
            #endif


            vector<string> lineSplit = split(current_line, " ", true);
            if( lineSplit.size() == 1)
                lineSplit.push_back(" ");
            int cmdOut = run_cmd(dumbHash(lineSplit[0]), lineSplit[1]);

            if (cmdOut != 0)
            {
                debug("There was an issue");
                debug(cmdOut);
                this->canRun = false;
                return;
            }

            this->program_counter++;
        }
    }

};

#endif
