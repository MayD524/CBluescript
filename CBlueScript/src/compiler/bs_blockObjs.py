from main import bsCompiler

class bsBlockObjects:
    def __init__(self, blockName:str, compiler_instance:bsCompiler, line:str):
        self.blockName = blockName
        self.compiler  = compiler_instance
        self.line      = line
        
        self.logicOpers = {
            "==": "je", 
            "!=": "jne", 
            "<=": "jge", ## just flip inputs
            ">=": "jge", 
            "<" : "jng", 
            ">" : "jg"}
        
        self.cmpLine = None
    
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
        
        if self.compiler.check_exists(exper_split[0].strip()):
            exper_split[0] = f"%{self.compiler.curScope[-1]}.{exper_split[0].strip()}"
        else:
            tmpVar =  self.compiler.gen_tmpVars()
            self.compiler.parsedLines.append(f"mov {self.compiler.curScope[-1]}.{tmpVar},{exper_split[0].strip()}")
            exper_split[0] = f"%{self.compiler.curScope[-1]}.{tmpVar}"

        if self.compiler.check_exists(exper_split[1].strip()):
            exper_split[1] = f"%{self.compiler.curScope[-1]}.{exper_split[1].strip()}"
        else:
            tmpVar =  self.compiler.gen_tmpVars()
            self.compiler.parsedLines.append(f"mov {self.compiler.curScope[-1]}.{tmpVar},{exper_split[1].strip()}")
            exper_split[1] = f"%{self.compiler.curScope[-1]}.{tmpVar}"

        if logic_oper == "<=":
            self.cmpLine = f"cmp {exper_split[1].strip()},{exper_split[0].strip()}"
        
        else:
            self.cmpLine = f"cmp {exper_split[0].strip()},{exper_split[1].strip()}"

        self.logicMode = self.logicOpers[logic_oper]
    
    def block_start(self) -> None:
        """
            The header for a block
            
            @params:
                None
            @returns:
                None
        """
        pass
    
    def block_end(self) -> None:
        """
            The tail for a block
            
            @params:
                None
            @returns:
                None
        """
        pass

class bsFuncObj(bsBlockObjects):
    def __init__(self, bsCompilerInstance:bsCompiler, line:str) -> None:
        super().__init__(bsCompilerInstance.gen_tmpVars(), bsCompilerInstance, line)
        self.funcName = ""
    
    def block_end(self) -> None:
        if self.funcName == "main":
            self.compiler.parsedLines.append(f"jmp EOF_BS_JMP_POINT")
            return

        self.compiler.parsedLines.append("ret ; return from function")

    def block_start(self) -> None:
        
        funcArgs = self.line.replace(")", '').split("(",1)
        
        self.compiler.parsedLines.append(f"label {funcArgs[0]} ; start of function") ## start of function
        self.funcName = funcArgs[0]
        params_split = funcArgs[1].split(',')[::-1]

        if params_split[0] == '': ## no params
            return
        self.funcArgs = params_split
        for param in params_split:
            self.compiler.parsedLines.append(f"pop {self.funcName}.{param.strip()}")
            self.compiler.declairedVars[f"{self.funcName}.{param.strip()}"] = "variable"

    
class bsWhileObj(bsBlockObjects):
    def __init__(self, bsCompilerInstance:bsCompiler, line:str) -> None:
        super().__init__(bsCompilerInstance.gen_tmpVars(), bsCompilerInstance, line)

    def block_end(self) -> None:
        """
            cmp %<ptr1>,%<ptr2>
            {jump} {start of while}
        """
        self.logicComp(self.line)
        self.compiler.parsedLines.append(self.cmpLine)
        self.compiler.parsedLines.append(f"{self.logicMode} {self.blockName} ; jump to head if we can continue iterating")
    
    def block_start(self) -> None:
        self.compiler.parsedLines.append(f"label {self.blockName} ; start of while loop")

class bsElseObj(bsBlockObjects):
    def __init__(self, bsCompilerInstance:bsCompiler, splitLine:str, prev_id:str) -> None:
        super().__init__(bsCompilerInstance.gen_tmpVars(), bsCompilerInstance, splitLine)
        self.prev_id = prev_id
    
    def block_end(self) -> None:
        self.compiler.parsedLines.append(f"label end_of_else_{self.blockName} ; end of else")

    def block_start(self) -> None:
        """
        else block
            mov {tmp},1
            cmp %{prev},%{tmp}
            jne {else_exit}
            ; code
            label {else_exit}
        """
        elseTmpVar = self.compiler.gen_tmpVars()
        self.compiler.parsedLines.append(f"mov {elseTmpVar},1 ; temp constant")
        self.compiler.parsedLines.append(f"cmp %{self.prev_id}_isSet,%{elseTmpVar} ; compair the isSet of the previous if block")
        self.compiler.parsedLines.append(f"je end_of_else_{self.blockName} ; jump if the previous if was true")
        


class bsIfObj(bsBlockObjects):
    def __init__(self, bsCompilerInstance:bsCompiler, splitLine:str) -> None:
        super().__init__(bsCompilerInstance.gen_tmpVars(), bsCompilerInstance, splitLine)
    
    def block_end(self) -> None:
        self.compiler.parsedLines.append(f"label end_of_if_{self.blockName} ; end of if")
    
    def block_start(self) -> None:
        """
        if block
            mov {block name}_isSet,0 ; if block check
            cmp %{1},%{2}
            {jump} {if_exit}
        """
        self.logicComp(self.line, True)
        self.compiler.parsedLines.append(f"mov {self.blockName}_isSet,0 ; start of an if block")
        self.compiler.parsedLines.append(f"{self.cmpLine} ; if block compairison")
        self.compiler.parsedLines.append(f"{self.logicMode} end_of_if_{self.blockName} ; jump to end if expression is false")
        self.compiler.parsedLines.append(f"mov {self.blockName}_isSet,1 ; we made it in the if statement")