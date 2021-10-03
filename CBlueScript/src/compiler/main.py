"""
    Author: May Draskovics
    Date: 9/23/2021
    version: 1.0.0
"""
import bs_blockObjs
import UPL
import sys

class bsCompiler:
    def __init__(self, filename:str, filelines:list) -> None:
        self.filename    = filename
        self.filelines   = filelines
        self.parsedLines = ["jmp main"]
        self.curScope    = ["main"]

        ## var_name : [var_code, type(func|vartypes|lable)]
        self.declairedVars = {}
        self.lineNo = 0
        
        self.blocks = []
        
        self.keyWords = [
            "if", 
            "else",
            "function",
            "while",
            "print",
            "input",
            "return",
            "free"
        ]
        
        self.mathOpers = ["*", "/", "+", "-"]
        self.logicOpers = {
            "==": "je", 
            "!=": "jne", 
            "<=": "jge", ## just flip inputs
            ">=": "jge", 
            "<" : "jng", 
            ">" : "jg"}

    def check_exists( self, varname:str ) -> bool:
        return True if f"{self.curScope[-1]}.{varname}" in self.declairedVars else False

    def containsMath( self, arg:str ) -> str:
        for oper in self.mathOpers:
            if oper in arg:
                return oper
            
        return False

    
    def gen_tmpVars( self ) -> str:
        var = UPL.Core.generate_code(7).replace(',', '!').replace("%", "1").replace(";", '2')
        ## should stop from duplicates happening
        while var in self.declairedVars.keys():
            var = UPL.Core.generate_code(7).replace(',', '!').replace("%", "1").replace(";","2")

        return var

    def parseMath( self, exper:str, mathOper:str ) -> str:
        generated_retVal = self.gen_tmpVars()
        exper = exper.split(mathOper, 1)
        exper = [x.strip() if "\"" not in x else x.replace("\"", '') for x in exper]
        
        if self.check_exists(exper[0]):
            exper[0] = f"%{self.curScope[-1]}.{exper[0]}"
            
        if self.check_exists(exper[1]):
            exper[1] = f"%{self.curScope[-1]}.{exper[1]}" 
        self.parsedLines.append(f"mov {self.curScope[-1]}.{generated_retVal},{exper[0]}")

        ## make it look better later
        ## this is the only one strings are allowed to use
        if mathOper == "+":
            self.parsedLines.append(f"add {self.curScope[-1]}.{generated_retVal},{exper[1]}")
        
        ## numeric types only
        elif mathOper == "-":
            self.parsedLines.append(f"sub {self.curScope[-1]}.{generated_retVal},{exper[1]}")
        
        elif mathOper == "*":
            self.parsedLines.append(f"mul {self.curScope[-1]}.{generated_retVal},{exper[1]}")
        
        elif mathOper == "/":
            self.parsedLines.append(f"div {self.curScope[-1]}.{generated_retVal},{exper[1]}")

        return generated_retVal

    def parseVarAssign( self, line:str ) -> None:
        lineSplit = line.split('=',1)
        varhasMath = self.containsMath(lineSplit[0])
        
        varName = ""
        varExpress = ""
        
        if "(" in lineSplit[1]:
            ## is a function
            funcParse = lineSplit[1].replace(")", '').split("(")
            funcName = funcParse[0].strip() ## the name of the function we want to call
            funcArgs = [x.strip() for x in funcParse[1].split(",")] ## get args (in order)

            if funcName in self.declairedVars.keys():
                ## the function exits!
                for arg in funcArgs:
                    self.parsedLines.append(f'push %{self.curScope[-1]}.{arg}' if self.check_exists(arg) else f'push {arg}')
                
                ## call the function
                self.parsedLines.append(f"call {funcName}")
                self.parsedLines.append(f"pop {self.curScope[-1]}.{lineSplit[0].strip()}")
                self.declairedVars[f"{self.curScope[-1]}.{lineSplit[0].strip()}"] = "variable"
            else:
                ## wtf the function doesn't exist
                raise Exception(f"Function {funcName} does not exist")

            return
        
        elif varhasMath:
            tmp = lineSplit[0].split(varhasMath)
            varExpress = lineSplit[0].strip() + lineSplit[1].strip()
            varName = tmp[0].strip()

        else:
            varName = lineSplit[0].strip()
            varExpress = lineSplit[1].strip().replace("\"", "")
            
        self.declairedVars[f"{self.curScope[-1]}.{varName}"] = "variable"
        
        expresHasMath = self.containsMath(varExpress)
        
        if expresHasMath:
            genRet = self.parseMath(varExpress, expresHasMath)
            self.parsedLines.append(f"mov {self.curScope[-1]}.{varName},%{self.curScope[-1]}.{genRet}")
            
        else:
            if self.check_exists(varExpress):
                varExpress = f"%{self.curScope[-1]}.{varExpress}"
                
            self.parsedLines.append(f"mov {self.curScope[-1]}.{varName},{varExpress}")
    
    def preCompiledHandle( self, lines:list ) -> None:
        labels = [ x for x in lines if x.startswith('label')]
        
        for label in labels:
            labelName = label.split(' ',1)[1]
            self.declairedVars[labelName] = 'function'
    
    def handle_includes( self, lines:list ) -> None:
        includes = [x for x in lines if x.startswith('include')]
        for include in includes:
            fileName = include.split(" ",1)[1]
            newLines = []
            if not fileName.endswith(".cbs"):
                newLines = [x.strip() for x in UPL.Core.file_manager.clean_read(fileName)]
                self.handle_includes(newLines)
                self.filelines = newLines + self.filelines

            else:
                newPreParsedLines = [x.strip() for x in UPL.Core.file_manager.clean_read(fileName)]
                self.parsedLines += newPreParsedLines
                self.preCompiledHandle(newPreParsedLines)

            
            
            self.filelines.remove(include)

    def compileLines( self ) -> None:
        self.handle_includes(self.filelines) ## deal with includes
        ## do includes before main compile & functions
        while self.lineNo != len(self.filelines):
            currentLine = self.filelines[self.lineNo].rsplit("//", 1)[0].strip()
            if "=" in currentLine and currentLine.split(' ',1)[0] not in self.keyWords:
                ## variable delic
                self.parseVarAssign(currentLine)
            
            elif any(currentLine.startswith(keyword) for keyword in self.keyWords):
                ## check for key words
                splitLine = currentLine.split(' ', 1)
                if splitLine[0] == "print":
                    if self.check_exists(splitLine[1]):
                        splitLine[1] = f"%{self.curScope[-1]}.{splitLine[1]}"
                    else:
                        tvar = self.gen_tmpVars()
                        self.parsedLines.append(f"mov {self.curScope[-1]}.{tvar},{splitLine[1]}")
                        splitLine[1] = f"%{self.curScope[-1]}.{tvar}"
                    self.parsedLines.append(f"out {splitLine[1]}")

                elif splitLine[0] == "free":
                    splitLine[1] = splitLine[1].strip()
                    if self.check_exists(splitLine[1]):
                        self.parsedLines.append(f"free %{self.curScope[-1]}.{splitLine[1]}")
                    else:
                        tmpVar = self.gen_tmpVars()
                        self.parsedLines.append(f"mov {self.curScope[-1]}.{tmpVar},{splitLine[1]}")
                        self.parsedLines.append(f'free %{self.curScope[-1]}.{tmpVar}')

                elif splitLine[0] == "return":
                    splitLine[1] = splitLine[1].strip()
                    if self.check_exists(splitLine[1]):
                        self.parsedLines.append(f"push %{self.curScope[-1]}.{splitLine[1]}")
                        self.parsedLines.append(f"free %{self.curScope[-1]}.{splitLine[1]}")
                        self.parsedLines.append('ret ; end of function')
                    else:
                        tmpVar = self.gen_tmpVars()
                        self.parsedLines.append(f"mov {self.curScope[-1]}.{tmpVar},{splitLine[1]}")
                        self.parsedLines.append(f"push %{self.curScope[-1]}.{tmpVar}")
                        self.parsedLines.append(f'free %{self.curScope[-1]}.{tmpVar}')
                        self.parsedLines.append('ret ; end of function')
                
                elif splitLine[0] == 'input':
                    ## input <prompt>,<return>
                    prompt, returnPointer = splitLine[1].split(",", 1)
                    prompt = prompt.strip()
                    returnPointer = returnPointer.strip()
                    if self.check_exists(prompt):
                        prompt = f"%{prompt}"
                    else:
                        tmpPromptVar = self.gen_tmpVars()
                        prompt = prompt.replace("\"", '')
                        self.parsedLines.append(f"mov {self.curScope[-1]}.{tmpPromptVar},{prompt}")
                        prompt = f"%{self.curScope[-1]}.{tmpPromptVar}"
                    
                    self.parsedLines.append(f"in {self.curScope[-1]}.{returnPointer},{prompt}")
                    self.declairedVars[f"{self.curScope[-1]}.{returnPointer}"] = "variable"

                elif splitLine[0] == "while":
                    tmpWhileObj = bs_blockObjs.bsWhileObj(self, splitLine[1])
                    tmpWhileObj.block_start()
                    self.blocks.append(tmpWhileObj)

                elif splitLine[0] == 'if':
                    tmpIfObj = bs_blockObjs.bsIfObj(self, splitLine[1])
                    tmpIfObj.block_start()
                    self.blocks.append(tmpIfObj)

                elif splitLine[0] == 'function':
                    tmpFuncObj = bs_blockObjs.bsFuncObj(self, splitLine[1])
                    tmpFuncObj.block_start()
                    self.declairedVars[tmpFuncObj.funcName] = "function"
                    self.curScope.append(tmpFuncObj.funcName)
                    self.blocks.append(tmpFuncObj)

            elif "(" in currentLine:
                ## function calls
                lineSplit = currentLine.replace(")","").split("(")
                funcArgs = lineSplit[1].split(',')
                for arg in funcArgs:
                    if arg not in self.declairedVars:
                        self.parsedLines.append(f"push %{self.curScope[-1]}.{arg}")
                    else:
                        self.parsedLines.append(f"push %{self.curScope[-1]}.{arg}")
                self.parsedLines.append(f"call {lineSplit[0]}")
                    
            ## jump to eof when this line is ran
            elif currentLine == "exit":
                self.parsedLines.append("jmp EOF_BS_JMP_POINT")

            elif currentLine == "}":
                if len(self.filelines) > self.lineNo + 1 and self.filelines[self.lineNo + 1].startswith("else"):
                    tmpElseBlock = bs_blockObjs.bsElseObj(self, splitLine, self.blocks[-1].blockName)
                    self.blocks[-1].block_end()
                    self.blocks.pop()
                    tmpElseBlock.block_start()
                    self.curScope.pop()
                    self.blocks.append(tmpElseBlock)
                
                else:
                    self.blocks[-1].block_end()
                    self.blocks.pop()

            self.lineNo += 1
        if "main" not in self.declairedVars.keys():
            raise Exception("No main function declaired")

        self.parsedLines.append("label EOF_BS_JMP_POINT") ## used for exiting later
        #self.parsedLines = [x for x in self.parsedLines if x is not None]
        print('\n'.join(self.parsedLines))


def main( filename:str ) -> None:
    fileLines = UPL.Core.file_manager.clean_read(filename)
    fileLines = [x.rsplit('//')[0].strip() for x in fileLines]
    compiler = bsCompiler(filename, fileLines)
    compiler.compileLines()

    writeFile = filename.replace(".bs", ".cbs")
    with open(writeFile, "w+"): pass
    UPL.Core.file_manager.write_file(writeFile, "\n".join(compiler.parsedLines))


if __name__ == "__main__":
    if len(sys.argv) > 0:
        filename = sys.argv[1]
        main(filename)