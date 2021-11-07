"""
    Author: May Draskovics
    Date: 10/21/2021
    version: 2.0b
"""
from UPL.Core import file_manager as fm
from UPL.Core import generate_code
from nltk.util import clean_url
from pprint import pprint
import bs_tokenizer
import sys

class bsCompiler:
    def __init__(self, filename:str, filelines:list, isLib:bool) -> None:
        self.filename       = filename
        self.filelines      = filelines
        self.isLib          = isLib
        self.parsedLines    = ["jmp main"] if not self.isLib else []
        self.curScope       = ["main"] if not self.isLib else ["generic"]

        self.declared       = ["main"]
        self.included_files = []

        self.logicInc       = 0
        self.blocks         = []
        self.blockNames     = []

        self.inMath         = False
        self.mathName       = None

        self.isGlobal       = False
        self.makeFunc       = False
        self.callFunc       = False
        self.isFuncArgs     = False
        self.makeVar        = False
        self.parseAsStr     = False
        self.swapMode       = False ## for logic swaps
        self.curLine        = None
        
        
        self.stringHold     = []
    
        self.funcArgParsed  = []
        
    def getNext(self, array:list, current:any) -> any:
        return array[array.index(current) + 1] if array.index(current) + 1 < len(array) else False
    
    def getPrev(self, array:list, current:any) -> any:
        return array[array.index(current) - 1]
    
    def isDeclared(self, varname:str) -> str:
        if (x := f"{self.curScope[-1]}.{varname}") in self.declared:
            return f"%{x}"
        elif (x := f"global.{varname}") in self.declared:
            return f"%{x}"
        else:
            return varname

    def pre_read(self, arr=None) -> None:
        if not arr:
            includes:list[str] = [x for x in self.filelines if x.startswith('include')]
        else:
            includes:list[str] = [x for x in arr if x.startswith('include')]

        for include in includes:
            fname = include.split(' ', 1)[1]
            if fname in self.included_files:
                continue
            
            if include.endswith(".bs"):
                readLines = fm.clean_read(fname)
                self.pre_read(readLines)
                self.filelines = readLines + self.filelines

            elif include.endswith('.cbs'):
                readLines = fm.clean_read(fname)
                self.parsedLines = self.parsedLines + readLines
    
    def genTmp(self, length:int) -> str:
        var = generate_code(length).replace(',', '!').replace("%", "1").replace(";", '2')
        ## should stop from duplicates happening
        while var in self.declared:
            var = generate_code(length).replace(',', '!').replace("%", "1").replace(";","2")

        return var

    @staticmethod
    def getOpposite(mode:str) -> str:
        match mode:
            case 'je':
                return 'jne'

            case 'jne':
                return 'je'
            
            case 'jng':
                return 'jg'
            
            case 'jg':
                return 'jng'
            
            case 'jge':
                return 'jge'
        
        ## return non-conditional jump incase of error
        return 'jmp'

    def compile(self) -> None:
        self.pre_read()
        tokenized = bs_tokenizer.tokenize_all(self.filelines)
        pprint(tokenized)
        skip = 0
        for line in tokenized:
            
            if bs_tokenizer.LPAR in line and bs_tokenizer.BS_FUNCTION not in line:
                self.callFunc = True
                expectReturn  = True if bs_tokenizer.EQL in line else False
                funcName      = line[line.index(bs_tokenizer.LPAR) - 1]
                funcArgs      = line[line.index(bs_tokenizer.LPAR) + 1:line.index(bs_tokenizer.RPAR)]
                print(funcArgs)
                continue
            
            print(line)
            
            for t in range(len(line)):
                if skip:
                    skip -= 1
                    continue
                token = line[t]
                if token in bs_tokenizer.BS_MATH_TOKENS:
                
                    self.inMath = True
                    match token:
                        case bs_tokenizer.MATH_ADDOP:
                            self.mathName = "add"
                        case bs_tokenizer.MATH_SUBOP:
                            self.mathName = "sub"
                        case bs_tokenizer.MATH_MULOP:
                            self.mathName = "mul"
                        case bs_tokenizer.MATH_DIVOP:
                            self.mathName = "div"
                    
                    continue
                
                elif token in bs_tokenizer.BS_LOGIC_TOKENS.keys():
                    mode = bs_tokenizer.BS_LOGIC_TOKENS[token]
                    if token == bs_tokenizer.LOGIC_JGEOPS: self.swapMode = True
                    mode = mode if self.blocks[-1][1] not in ['logic_if', 'logic_elif'] else self.getOpposite(mode)
                    self.blocks[-1].append(mode)
                    if len(self.stringHold) > 0:
                        tmpVar = f'{self.curScope[-1]}.{self.genTmp(5)}'
                        self.parsedLines.append(f'mov {tmpVar},{" ".join(self.stringHold)}')
                        self.declared.append(tmpVar)
                        self.curLine += f"%{tmpVar}"
                        self.stringHold = []
                    self.curLine += ','
                    continue
                
                match token:
                    
                    case bs_tokenizer.BSEOF:
                        continue
                    
                    case bs_tokenizer.BS_FUNCTION:
                        self.makeFunc = True
                        continue

                    case bs_tokenizer.LPAR:
                        if self.makeFunc:
                            self.isFuncArgs = True
                        continue

                    case bs_tokenizer.RPAR:
                        if self.makeFunc == True:
                            self.makeFunc = False
                            self.isFuncArgs = False

                            ## reverse as we pop in reverse of how we pushed
                            self.funcArgParsed.reverse()

                            for argline in self.funcArgParsed:
                                self.parsedLines.append(argline)

                            self.funcArgParsed = [] ## reset the array
                            
                        continue

                    ## ignore these unless in context?
                    case bs_tokenizer.COMMA | bs_tokenizer.QUOTE | bs_tokenizer.CLBAR:
                        continue
                    
                    case bs_tokenizer.NEWOP: 
                        ## reset flags
                        self.makeFunc   = False
                        self.isFuncArgs = False
                        self.makeVar    = False
                        self.parseAsStr = False

                        if len(self.stringHold) > 0 and self.callFunc:
                            self.parsedLines.append(f'push {" ".join(self.stringHold)}')
                            self.stringHold = []
                        
                        if self.callFunc and expectReturn:
                            varname = self.isDeclared(self.curLine.split(' ', 1)[1].split('.',1)[1].replace(',', ''))
                            self.curLine = f'pop {varname}'
                        
                        self.callFunc   = False
                        
                        if self.inMath:
                            self.curLine = self.curLine.replace("mov", self.mathName)
                            self.mathName = None
                            self.inMath = False
                        
                        if len(self.stringHold) > 0:
                            tmp = ' '.join(self.stringHold)
                            tmpVar = self.genTmp(5)
                            self.parsedLines.append(f'mov {self.curScope[-1]}.{tmpVar},{tmp}')
                            self.curLine += f'%{self.curScope[-1]}.{tmpVar}'
                            self.stringHold = []
                        
                        ## append new line
                        self.parsedLines.append(self.curLine)
                        if self.curLine and self.curLine.startswith('cmp'):
                            if self.swapMode: self.parsedLines.append('not grt'); self.swapMode = False
                            if self.blocks[-1][1] != 'logic_while':
                                self.parsedLines.append(f'{self.blocks[-1][3]} {self.blocks[-1][2]}')
                                self.parsedLines.append(f'mov {self.blocks[-1][0]}_isSet,1')

                        self.curLine    = None
                        continue        
                    
                    case bs_tokenizer.BS_PRINT:
                        self.curLine = "out"
                        continue

                    case bs_tokenizer.CRBAR:
                        
                        if self.curScope[-1] != 'main' and self.logicInc == 0:
                            self.parsedLines.append("ret")
                        
                        elif len(self.blocks) > 0 and self.blocks[-1][1].startswith('logic'):
                            if self.blocks[-1][1] != 'logic_while':
                                self.parsedLines.append(f'label {self.blocks[-1][2]}')
                                if line[t + 2] not in [bs_tokenizer.BS_ELIFKW, bs_tokenizer.BS_ELSEKW]:
                                    self.blockNames.pop() ## block namespace ends here
                                self.blocks.pop()
                                self.logicInc -= 1
                                continue
                            
                            
                            self.parsedLines.append(f'{self.blocks[-1][3]} {self.blocks[-1][2]}')
                            self.blocks.pop()
                            
                        else:
                            self.parsedLines.append("jmp BS_EOF_JUMP_POINT")
                    
                    case bs_tokenizer.BS_IFKW:
                        blockName = self.genTmp(20)
                        self.blockNames.append(blockName)
                        self.blocks.append([blockName, 'logic_if', f'{blockName}_endOfIf'])
                        self.parsedLines.append(f'mov {blockName}_isSet,0')
                        self.curLine = "cmp "
                        self.logicInc += 1
                        
                    case bs_tokenizer.BS_ELIFKW:
                        blockName = self.blockNames[-1]
                        self.blocks.append([blockName, 'logic_elif', f'{blockName}_endOfelif'])
                        self.parsedLines.append(f'mov {blockName}_const,1')
                        self.parsedLines.append(f'cmp %{blockName}_isSet,%{blockName}_const')
                        self.parsedLines.append(f'free %{blockName}_const')
                        self.parsedLines.append(f'je {blockName}_endOfelif')
                        
                        self.curLine = "cmp "
                        self.logicInc += 1
                    
                    case bs_tokenizer.BS_ELSEKW:
                        blockName = self.blockNames[-1]
                        self.blocks.append([blockName, 'logic_else', f'{blockName}_endOfelse'])
                        self.parsedLines.append(f'mov {blockName}_const,1')
                        self.parsedLines.append(f'cmp %{blockName}_isSet,%{blockName}_const')
                        self.parsedLines.append(f'free %{blockName}_const')
                        self.parsedLines.append(f'je {blockName}_endOfelse')
                    
                    case bs_tokenizer.BS_GLOBAL:
                        self.isGlobal = True
                    
                    case bs_tokenizer.BS_WHILEKW:
                        blockName = self.genTmp(20)
                        self.blocks.append([blockName, 'logic_while', f'{blockName}_whileBlock'])
                        self.parsedLines.append(f'label {blockName}_whileBlock')
                        self.curLine = 'cmp '
                        self.logicInc += 1
                    
                    case bs_tokenizer.BS_RETURN:
                        if (nxt := self.isDeclared(line[t + 1])).startswith('%'):
                            self.parsedLines.append(f"push {nxt}")
                            continue
                        self.curLine = "push "
                        skip += 1
                    
                    case _:         
                        if self.curLine and (self.curLine.startswith('cmp') or self.curLine.startswith('push')):
                            var = self.isDeclared(token)
                            if var.startswith('%'):
                                self.curLine += f'{var}'
                            else:
                                self.stringHold.append(var)
                            continue

                        if self.makeFunc and not self.isFuncArgs:
                            ## function name
                            self.parsedLines.append(f"label {token}")
                            self.curScope.append(token)
                            continue
                        
                        elif self.makeFunc and self.isFuncArgs:
                            self.funcArgParsed.append(f"pop {self.curScope[-1]}.{token}")
                            self.declared.append(f"{self.curScope[-1]}.{token}")
                            continue
                            
                        elif self.curLine and type(token) == str:
                            if not self.callFunc:
                                if not self.curLine.endswith(','):
                                    self.curLine += f" {self.isDeclared(token)}" if any(x.isalnum() for x in token) else token
                                else:
                                    self.curLine += self.isDeclared(token)
                                continue
                            
                            if line[t + 1] == bs_tokenizer.COMMA or line[t + 1] == bs_tokenizer.RPAR:
                                self.parsedLines.append(f"push {self.isDeclared(token)}")
                                
                            else:
                                self.stringHold.append(token)
                            continue

                        elif self.makeVar and not self.curLine:
                            if (nt := line[t + 1]) == str:
                                self.curLine += self.isDeclared(nt)
                                continue   
                        else:
                            if ( nt:= line[t + 1]) == bs_tokenizer.EQL and not self.curLine:
                                ## var delc
                                self.makeVar = True
                                if any(item in line for item in bs_tokenizer.BS_MATH_TOKENS):
                                    self.parsedLines.append(f"mov {self.curScope[-1]}.{token},{self.isDeclared(nxt)}" if not self.isGlobal else f"global.{token}")
                                    self.declared.append(f"{self.curScope[-1]}.{token}" if not self.isGlobal else f"global.{token}")
                                    skip += 2
                                    
                                    
                                self.curLine = f"mov {self.curScope[-1] if not self.isGlobal else 'global'}.{token},"
                                self.declared.append(f"{self.curScope[-1]}.{token}" if not self.isGlobal else f"global.{token}")

                            elif nt == bs_tokenizer.LPAR:
                                ## func call
                                self.curLine = f"call {token}"
                                self.callFunc = True

        self.parsedLines.append("label BS_EOF_JUMP_POINT")
        self.parsedLines = [x for x in self.parsedLines if x]
        print(tokenized)
        #print('\n'.join(self.parsedLines))
        



def main( filename:str, isLib:bool ) -> None:
    fileLines = fm.clean_read(filename)
    fileLines = [x.rsplit("//")[0].strip() for x in fileLines]
    compiler = bsCompiler(filename, fileLines, isLib)
    compiler.compile()
    
    writeFile = filename.replace('.bs', '.cbs')
    with open(writeFile, "w+"): pass
    fm.write_file(writeFile, "\n".join(compiler.parsedLines))
        
        
if __name__ == "__main__":
    if (len(sys.argv) > 0):
        filename = sys.argv[1]
    isLib = False if "lib" not in sys.argv else True
    main(filename, isLib)
