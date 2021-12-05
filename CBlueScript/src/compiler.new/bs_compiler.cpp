#include "bs_compiler.h"


string_vector BS::cleanComments(string_vector lines) {
  string_vector cleanLines;
  for (int i = 0; i < lines.size(); i++) {
    string line = lines[i];
    line = line.substr(0, line.find("//"));
    line = trim(line);
    cout << line << endl;
    cleanLines.push_back(line);
  }
  return cleanLines;
}

string makeVariableName(const string & scope, const string & name) {
  return scope + "_" + name;
}

bool varExists(const string & name, const variable_vector & decl) {
  for (const auto &
      var: decl) {
    if (var.name == name)
      return true;
  }
  return false;
}

bool varIsConst(const string & name, const variable_vector & decl) {
  for (const auto &
      var: decl) {
    if (var.name == name)
      return var.isConst;
  }
  return false;
}

void BS::Compile(cstrref fileName, cstrref outputFileName) {
  string_vector lines = cleanComments(readFile(fileName));
  tokenizer tk = tokenizer(lines);
  vector < token_vector > tokens = tk.tokenize();
  block_vector blocks;

  bool createFunction = false;
  bool readingFuncArgs = false;
  bool isConst = false;
  bool isGlobal = false;

  string funcName;
  string curLine = "";
  string_vector function_args;
  string_vector scopes;
  variable_vector declaredVars;

  scopes.push_back("main");
  //printf("here\n");
  try {
  for (int i = 0; i < tokens.size(); i++) {
    curLineNo += 1;
    printf("\n[%i]", i);
    for (int j = 0; j < tokens[i].size(); j++) {
      printf("[%i:%i]", i, j);
      token t = tokens[i][j];
      if (t.isCmd) {
        cout << t.ivalue << " ";
        switch (t.ivalue) {
          case BS::BS_FUNCTION:
            {
              createFunction = true;
            }
            break;

          case BS::LPAR:
            {
              readingFuncArgs = true;
            }
            break;

          case BS::RPAR:
            {
              readingFuncArgs = false;
              // write and reset function args
            }
            break;

          case BS::BS_FORKW:
          case BS::BS_WHILEKW:
            break;

          case BS::NEWOP:
            {
              if (createFunction) {
                
                printf("'%s'\n", funcName.c_str());
                // format a string
                comp_lines.push_back("label " + funcName);
                BS_block b;
                b.name = funcName;
                b.type = 0;
                
                blocks.push_back(b);
                for (int k = 0; k < function_args.size(); k++) {
                  curLine += function_args[k];
                  string varName = makeVariableName(funcName, function_args[k]);
                  if (!varIsConst(varName, declaredVars)) {
                    bs_variable var = bs_variable(varName, isConst);
                    comp_lines.push_back("pop " + varName);
                    declaredVars.push_back(var);
                  }
                }
                function_args.clear();
                createFunction = false;
              }
              if (curLine.size() > 0) {
                comp_lines.push_back(curLine);
                curLine = "";
              }
            }
            break;

          case BS::BS_PRINT:
            {
              curLine += "out ";
            }
            break;

            // end of block
          case BS::CRBAR:
            {
              // this is what broke everything...
              // dont change this...
              if (blocks.size() <= 0) { continue; }
              BS_block b = blocks.back();
              blocks.pop_back();
              b.block_end(comp_lines);
              if (createFunction)
                createFunction = false;
              scopes.pop_back();
            }
            break;
          case BS::BSEOF:
            { 
              // return lines
              printf("\nI have finished compiling\n");
              return;
            }
            break;
          default:
            break;
        }
        } else {
          cout << t.svalue << " ";
          if (readingFuncArgs)
            function_args.push_back(t.svalue);

          if (createFunction && !readingFuncArgs) {
            funcName = t.svalue;
            scopes.push_back(funcName);
          }

          if (curLine.size() > 0) {
            // check if curline starts with mov
            if (curLine.rfind("mov", 0) == 0) {
              // check if variable is already declared
              // if not, add it to the list of declared variables
              // if it is, check if it is a constant
              // if it is, error 
              // if it is not, add it to the list of declared variables
              string varName = t.svalue;
              if (varExists(makeVariableName(scopes.back(), varName), declaredVars)) 
                varName = "%" + makeVariableName(scopes.back(), varName);
              curLine += varName;
                    
              comp_lines.push_back(curLine);
            } 
            else if (curLine.rfind("out", 0) == 0) {
              string varName = t.svalue;
              if (varExists(makeVariableName(scopes.back(), varName), declaredVars))
                varName = "%" + makeVariableName(scopes.back(), varName);
              curLine += varName;
              comp_lines.push_back(curLine);
            }
            else { continue; }
            curLine += t.svalue;
            
          }

          // check what the next token is
          else if (j + 1 < tokens[i].size()) {
            token next = tokens[i][j + 1];
            if (next.isCmd) {
              string varName = makeVariableName(scopes.back(), t.svalue);
              if (!varIsConst(varName, declaredVars)) {
                  bs_variable var = bs_variable(varName, isConst);
                  //comp_lines.push_back("pop " + varName);
                  declaredVars.push_back(var);

                  if (next.ivalue == BS::EQL)
                    curLine = "mov " + varName + ",";
                  else if (anyInVector < int > (next.ivalue, BS::BS_MATH_TOKENS))
                  {
                    curLine = "mov " + varName + ",";
                    if (j + 2 > tokens[i].size()) { continue;}
                    token next2 = tokens[i][j + 2];
                    if (!next2.isCmd) {
                      curLine += next2.svalue;
                      j ++;
                    }
                  }
                } else {
                  cerr << "Error: variable " << varName << " was declaired as a constant variable." << endl;
                  exit(1);
                }
              j ++;
            }

          }
      }
    }
  }
  }
  catch (const exception & e) {
    cerr << "Error: " << e.what() << endl;
    cout << "-----------\nLines: " << endl;
    for (cstrref line: comp_lines)
      cout << line << endl;

    cout << "----------\nScopes: " << endl;
    for (cstrref scope: scopes)
      cout << scope << endl;

    cout << "----------\nVariables: " << endl;
    for (bs_variable var: declaredVars)
      cout << var.name << " " << var.isConst << endl;

    cout << "----------\nBlocks: " << endl;
    for (BS_block b: blocks)
      cout << b.name << endl;

    cout << "----------\nCurrent Flag states: " << endl;
    cout << "createFunction: " << createFunction << endl;
    cout << "readingFuncArgs: " << readingFuncArgs << endl;
    cout << "curLine: " << curLine << endl;
    cout << "funcName: " << funcName << endl;
  }
  cout << "here" << endl;
  return;
}
