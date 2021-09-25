"""
    Author: May Draskovics
    Date: 9/23/2021
    version: 1.0.0
"""
import UPL
import sys

class bsCompiler:
    def __init__(self, filename:str, filelines:list) -> None:
        self.filename   = filename
        self.filelines  = filelines
        self.parsedLines = []

        ## var_name : [var_code, type(func|vartypes|lable)]
        self.declairedVars = {}
        self.lineNo = 0

        self.blockID = ""
        self.inBlock = False

        self.logicMode  = ""
        self.logicBased = False
        self.isIf       = False
        self.hasElse = False
        
        self.keyWords = [
            "if", 
            "else",
            "function",
            "while",
            "print"
        ]
        
        self.mathOpers = ["*", "/", "+", "-"]
        self.logicOpers = {
            "==": "je", 
            "!=": "jne", 
            "<=": "jge", ## just flip inputs
            ">=": "jge", 
            "<" : "jng", 
            ">" : "jg"}

    def containsMath( self, arg:str ) -> str:
        for oper in self.mathOpers:
            if oper in arg:
                return oper
            
        return False

    def getLogicOper( self, exper:str, opposite:bool=False ) -> str:
        for oper in self.logicOpers.keys():
            if oper in exper:
                if opposite == True:
                    logicOps = list(self.logicOpers.keys())
                    operIndex = logicOps.index(oper)
                    
                    if operIndex % 2 == 0:
                        tmpOper = logicOps[operIndex + 1]
                    else:
                        tmpOper = logicOps[operIndex - 1]
            
                    return (oper, tmpOper)
            
                return oper

        return False
    
    def gen_tmpVars( self ) -> str:
        var = UPL.Core.generate_code(7).replace(',', '!').replace("%", "1")
        ## should stop from duplicates happening
        while var in self.declairedVars.keys():
            var = UPL.Core.generate_code(7).replace(',', '!').replace("%", "1")

        return var

    def logicComp( self, exper:str, opposite:bool=False ) -> str:
        logic_oper = self.getLogicOper(exper, opposite)

        if logic_oper == False:
            ## eval true or false raw expression
            return
        
        if opposite == False:
            exper_split = exper.split(logic_oper)
        else:
            exper_split = exper.split(logic_oper[0])
            logic_oper = logic_oper[1]
        
        if exper_split[0].strip() in self.declairedVars.keys():
            exper_split[0] = f"%{exper_split[0].strip()}"
        else:
            tmpVar = self.gen_tmpVars()
            self.parsedLines.append(f"mov {tmpVar},{exper_split[0].strip()}")
            exper_split[0] = f"%{tmpVar}"

        if exper_split[1].strip() in self.declairedVars.keys():
            exper_split[1] = f"%{exper_split[1].strip()}"
        else:
            tmpVar = self.gen_tmpVars()
            self.parsedLines.append(f"mov {tmpVar},{exper_split[1].strip()}")
            exper_split[1] = f"%{tmpVar}"

        if logic_oper == "<=":
            self.cmpLine = f"cmp {exper_split[1].strip()},{exper_split[0].strip()}"
        
        else:
            self.cmpLine = f"cmp {exper_split[0].strip()},{exper_split[1].strip()}"

        self.logicMode = self.logicOpers[logic_oper]


    def parseMath( self, exper:str, mathOper:str ) -> str:
        generated_retVal = self.gen_tmpVars()
        exper = exper.split(mathOper, 1)
        exper = [x.strip() if "\"" not in x else x.replace("\"", '') for x in exper]
        
        if exper[0] in self.declairedVars.keys():
            exper[0] = f"%{exper[0]}"
            
        if exper[1] in self.declairedVars.keys():
            exper[1] = f"%{exper[1]}" 
        self.parsedLines.append(f"mov {generated_retVal},{exper[0]}")

        ## make it look better later
        ## this is the only one strings are allowed to use
        if mathOper == "+":
            self.parsedLines.append(f"add {generated_retVal},{exper[1]}")
        
        ## numeric types only
        elif mathOper == "-":
            self.parsedLines.append(f"sub {generated_retVal},{exper[1]}")
        
        elif mathOper == "*":
            self.parsedLines.append(f"mul {generated_retVal},{exper[1]}")
        
        elif mathOper == "/":
            self.parsedLines.append(f"div {generated_retVal},{exper[1]}")

        return generated_retVal

    def parseVarAssign( self, line:str ) -> None:
        lineSplit = line.split('=',1)
        varhasMath = self.containsMath(lineSplit[0])
        
        varName = ""
        varExpress = ""
        
        if "(" in lineSplit[1]:
            ## is a function
            
            return
        
        elif varhasMath:
            tmp = lineSplit[0].split(varhasMath)
            varExpress = lineSplit[0].strip() + lineSplit[1].strip()
            varName = tmp[0].strip()

        else:
            varName = lineSplit[0].strip()
            varExpress = lineSplit[1].strip().replace("\"", "")
            
        self.declairedVars[varName] = "variable"
        
        expresHasMath = self.containsMath(varExpress)
        
        if expresHasMath:
            genRet = self.parseMath(varExpress, expresHasMath)
            self.parsedLines.append(f"mov {varName},%{genRet}")
            
        else:
            if varExpress in self.declairedVars.keys():
                varExpress = f"%{varExpress}"
                
            self.parsedLines.append(f"mov {varName},{varExpress}")
    
    def compileLines( self ) -> None:
        ## do includes before main compile & functions
        while self.lineNo != len(self.filelines):
            currentLine = self.filelines[self.lineNo].rsplit("//", 1)[0].strip()
            if "=" in currentLine and currentLine.split(' ',1)[0] not in self.keyWords:
                ## variable delic
                self.parseVarAssign(currentLine)
            
            elif "(" in currentLine:
                ## function calls
                pass
            
            elif any(currentLine.startswith(keyword) for keyword in self.keyWords):
                ## check for key words
                splitLine = currentLine.split(' ', 1)
                
                if splitLine[0] == "print":
                    if splitLine[1] in self.declairedVars.keys():
                        splitLine[1] = f"%{splitLine[1]}"
                    else:
                        tvar = self.gen_tmpVars()
                        self.parsedLines.append(f"mov {tvar},{splitLine[1]}")
                        splitLine[1] = f"%{tvar}"
                    self.parsedLines.append(f"out {splitLine[1]}")
                
                elif splitLine[0] == "while":
                    self.blockID = self.gen_tmpVars()
                    self.parsedLines.append(f"label {self.blockID}")
                    self.inBlock = True
                    self.logicBased = True

                    self.logicComp(splitLine[1])

                elif splitLine[0] == "if":
                    tmp = self.gen_tmpVars()
                    self.blockID = tmp
                    self.logicComp(splitLine[1], True)

                    self.parsedLines.append(self.cmpLine)
                    del self.cmpLine

                    self.isIf = True
                    self.parsedLines.append(f"mov {tmp}_isSet,0")
                    self.parsedLines.append(f"{self.logicMode} end_of_if_{self.blockID}")
                    self.parsedLines.append(f"mov {tmp}_isSet,1")

            ## jump to eof when this line is ran
            elif currentLine == "exit":
                self.parsedLines.append("jmp EOF_BS_JMP_POINT")

            elif currentLine == "}" :
                ## does not have an else block next
                self.inBlock = False

                if self.logicBased == True:
                    if self.cmpLine != None:
                        self.parsedLines.append(self.cmpLine)
                        del self.cmpLine
                        
                    self.parsedLines.append(f"{self.logicMode} {self.blockID}")
                        
                if self.isIf == True:
                    if self.lineNo + 1 < len(self.filelines) and self.filelines[self.lineNo + 1].startswith('else'):
                        tmpVar = self.gen_tmpVars()
                        self.parsedLines.append(f"label end_of_if_{self.blockID}")
                        self.parsedLines.append(f"mov {tmpVar},1")
                        self.parsedLines.append(f"cmp {self.blockID}_isSet,%{tmpVar}")
                        self.parsedLines.append(f"je end_of_else_{self.blockID}")
                        self.hasElse = True
                    else:
                        if self.hasElse:
                            self.parsedLines.append(f"label end_of_else_{self.blockID}")
                            self.hasElse = False
                        else:
                            self.parsedLines.append(f"label end_of_if_{self.blockID}")
                        
                        self.isIf = False
                        self.blockID = ""
                
                else:
                    self.isIf = False
                    self.blockID = ""
                
                self.logicBased = False
            else:
                ## is an else block
                pass
            self.lineNo += 1
        
        self.parsedLines.append("label EOF_BS_JMP_POINT") ## used for exiting later
        print('\n'.join(self.parsedLines))

def main( filename:str ) -> None:
    fileLines = UPL.Core.file_manager.clean_read(filename)

    compiler = bsCompiler(filename, fileLines)
    compiler.compileLines()


if __name__ == "__main__":
    if len(sys.argv) > 0:
        filename = sys.argv[1]
        main(filename)
