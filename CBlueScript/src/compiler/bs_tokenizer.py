import nltk
## keyword decl

## logic operations
BS_IFKW      = 0x10
BS_ELIFKW    = 0x11
BS_ELSEKW    = 0x12

## loops
BS_WHILEKW   = 0x20
BS_FORKW     = 0x21

## general ops
BS_FREE      = 0x30
BS_PRINT     = 0x31
BS_INPUT     = 0x32
BS_RETURN    = 0x33
BS_FUNCTION  = 0x34
BS_GLOBAL    = 0x35
BS_CONST     = 0x36

## OPERS
MATH_ADDOP   = 0x60
MATH_SUBOP   = 0x61
MATH_MULOP   = 0x62
MATH_DIVOP   = 0x63

LOGIC_JEOP   = 0x70
LOGIC_JNEOP  = 0x71
LOGIC_JGEOP  = 0x72
LOGIC_JNGOP  = 0x73
LOGIC_JGOP   = 0x74
LOGIC_JGEOPS = 0x75

## symboles
LPAR         = 0x100
RPAR         = 0x101
LBAR         = 0x102
RBAR         = 0x103
CLBAR        = 0x104
CRBAR        = 0x105
QUOTE        = 0x106
EQL          = 0x107
COMMA        = 0x108
BSLASH       = 0x109

NULL         = 0x0
NEWOP        = 0x200
NUMASSTR     = 0x201

BSEOF        = 0x999

BS_MATH_TOKENS = [
    MATH_DIVOP,
    MATH_ADDOP,
    MATH_MULOP,
    MATH_SUBOP
]

BS_LOGIC_TOKENS = {
    LOGIC_JEOP  : 'je',
    LOGIC_JNEOP : 'jne',
    LOGIC_JGEOP : 'jge',
    LOGIC_JNGOP : 'jng',
    LOGIC_JGOP  : 'jg',
    LOGIC_JGEOPS: 'jge'
}

BS_BINDS = {
    ## keywords
    "if"       : BS_IFKW,
    "elif"     : BS_ELIFKW,
    "else"     : BS_ELSEKW,
    "function" : BS_FUNCTION,
    "while"    : BS_WHILEKW,
    "for"      : BS_FORKW,
    "Print"    : BS_PRINT,
    "Input"    : BS_INPUT,
    "return"   : BS_RETURN,
    "free"     : BS_FREE,
    "const"    : BS_CONST,
    "global"   : BS_GLOBAL,
    
    ## math signs
    "*"        : MATH_MULOP,
    "/"        : MATH_DIVOP,
    "+"        : MATH_ADDOP,
    "-"        : MATH_SUBOP,

    "+="       : (EQL, MATH_ADDOP),
    "-="       : (EQL, MATH_SUBOP),
    "*="       : (EQL, MATH_MULOP),
    "/="       : (EQL, MATH_DIVOP),

    ## logic sings
    "=="       : LOGIC_JEOP,
    "!="       : LOGIC_JNEOP,
    "<="       : LOGIC_JGEOPS,
    ">="       : LOGIC_JGEOP,
    "<"        : LOGIC_JNGOP,
    ">"        : LOGIC_JGOP,
    
    "("        : LPAR,
    ")"        : RPAR,
    "{"        : CLBAR,
    "}"        : CRBAR,
    "\""       : QUOTE,
    "["        : LBAR,
    "]"        : RBAR,
    "="        : EQL,
    ","        : COMMA,
    "\\"       : BSLASH,
    "``"       : QUOTE,
    "''"       : QUOTE,
    ";"        : NEWOP,
}

#nltk.download("punkt")

HALF_LOGIC = ['<', '>', '!']

def tokenize_all(file_lines:list[str]) -> list[int]:
    tokenized_lines:list[int] = []
    inStr = False
    inFuncArgs = False
    for line in file_lines:
        lineTokens = nltk.word_tokenize(line)
        for token in lineTokens:
            if token in HALF_LOGIC:
                if lineTokens[lineTokens.index(token) + 1] == '=':
                    token += lineTokens[lineTokens.index(token) + 1] 
                    lineTokens.remove('=')
                    
            if token in BS_BINDS:
                if (t := BS_BINDS[token]) == 262:
                    inStr = not inStr
                    continue
                if inStr:
                    tokenized_lines.append(token)
                    continue

                if (type(t) == tuple):
                    for x in t:
                        tokenized_lines.append(x)
                    continue

                if (t == LPAR):
                    inFuncArgs = True

                elif (t == RPAR):
                    inFuncArgs = False

                if (inFuncArgs and t == COMMA):
                    continue
                
                tokenized_lines.append(t)
                
            else:
                tokenized_lines.append(token)
        
        tokenized_lines.append(NEWOP)

    tokenized_lines.append(BSEOF)
    return tokenized_lines
