#include "bs_compiler.h"


string_vector BS::cleanComments(string_vector lines)
{
    string_vector cleanLines;
    for (int i = 0; i < lines.size(); i++)
    {
        string line = lines[i];
        line = line.substr(0, line.find("//"));
        line = trim(line);
        cout << line << endl;
        cleanLines.push_back(line);
    }
    return cleanLines;
}

string makeVariableName(const string& scope, const string& name)
{
    return scope + "_" + name;
}

void BS::Compile(cstrref fileName, cstrref outputFileName)
{
    string_vector comp_lines;
    string_vector lines = cleanComments(readFile(fileName));
    tokenizer tk = tokenizer(lines);
    vector<token_vector> tokens = tk.tokenize();
    block_vector blocks;

    bool createFunction  = false;
    bool readingFuncArgs = false;
    string funcName;
    string curLine = "";
    string_vector function_args;
    string_vector scopes;
    scopes.push_back("main");

    for (int i = 0; i < tokens.size(); i++)
    {
        for (int j = 0; j < tokens[i].size(); j++)
        {
            token t = tokens[i][j];
            if (t.isCmd)
            {
                //cout << t.ivalue << " ";
                switch (t.ivalue)
                {
                    case BS::BS_FUNCTION:
                        createFunction = true;
                        break;

                    case BS::LPAR:
                        readingFuncArgs = true;
                        break;

                    case BS::RPAR:
                        readingFuncArgs = false;
                        // write and reset function args
                        break;

                    case BS::BS_FORKW:
                    case BS::BS_WHILEKW:
                        break;

                    case BS::NEWOP:
                        if (createFunction)
                        {
                            // format a string
                            comp_lines.push_back("label " + funcName);
                            BS_block b = BS_block(funcName, 0);
                            blocks.push_back(b);
                            for (int k = 0; k < function_args.size(); k++)
                            {
                                curLine += function_args[k];
                                comp_lines.push_back("pop " + makeVariableName(scopes.back(), function_args[k]));
                            }
                            function_args.clear();
                        }
                        if (curLine.size() > 0)
                        {
                            comp_lines.push_back(curLine);
                            curLine = "";
                        }
                        if (function_args.size() > 0)
                        {
                            comp_lines.push_back("pop " + function_args[0]);
                            function_args.erase(function_args.begin());
                        }
                        break;
                    
                    case BS::BS_PRINT:
                        curLine += "out ";
                        break;

                    // end of block
                    case BS::CRBAR:
                        BS_block b = blocks.back();
                        blocks.pop_back();
                        b.block_end(comp_lines);
                        if (createFunction)
                            createFunction = false;
                        scopes.pop_back();
                        break;
                }
            }
            else
            {
                if (readingFuncArgs)
                    function_args.push_back(t.svalue);
                
                if (createFunction && !readingFuncArgs)
                {
                    funcName = t.svalue;
                    scopes.push_back(funcName);
                }

                if (curLine.size() > 0)
                    curLine += t.svalue + " ";

                //cout << t.svalue << " ";
            }
        }
        //cout << endl;
    }

    for (cstrref line : comp_lines)
        cout << line << endl;
}
