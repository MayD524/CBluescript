#pragma once

#ifndef BS_TOKENIZER_H
#define BS_TOKENIZER_H

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <map>

#include "bs_defines.h"

using namespace std;

namespace BS
{
    enum tokens {
	BS_NULL,
	BS_IFLW,
        BS_ELIFKW,
        BS_ELSEKW,
        BS_WHILEKW,
        BS_FORKW,
        BS_FREE,
        BS_PRINT,
        BS_INPUT,
        BS_RETURN,
        BS_FUNCTION,
        BS_GLOBAL,
        BS_CONST,

        MATH_ADDOP,
        MATH_SUBOP,
        MATH_MULOP,
        MATH_DIVOP,
        
        LOGIC_JEOP,
        LOGIC_JNEOP,  
        LOGIC_JGEOP,  
        LOGIC_JNGOP,  
        LOGIC_JGOP,  
        LOGIC_JGEOPS, 

        LPAR,
        RPAR,
        LBAR,
        RBAR,
        CLBAR,
        CRBAR,
        QUOTE,
        EQL,
        COMMA,
        BSLASH,

        NEWOP,
        NUMASSTR,

        BSEOF
    };

    inline int_vector BS_MATH_TOKENS = {
        MATH_ADDOP,
        MATH_SUBOP,
        MATH_MULOP,
        MATH_DIVOP
    };

    inline map<int, string> BS_LOGIC_TOKENS = {
        {LOGIC_JEOP, "je"},
        {LOGIC_JNEOP, "jne"},
        {LOGIC_JGEOP, "jge"},
        {LOGIC_JNGOP, "jng"},
        {LOGIC_JGOP, "jg"},
        {LOGIC_JGEOPS, "jge"}
    };

    inline map<string, int> BS_BINDS = {
        {"if", BS_IFLW},
        {"elif", BS_ELIFKW},
        {"else", BS_ELSEKW},
        {"while", BS_WHILEKW},
        {"for", BS_FORKW},
        {"free", BS_FREE},
        {"Print", BS_PRINT},
        {"Input", BS_INPUT},
        {"return", BS_RETURN},
        {"function", BS_FUNCTION},
        {"global", BS_GLOBAL},
        {"const", BS_CONST},
        {"+", MATH_ADDOP},
        {"-", MATH_SUBOP},
        {"*", MATH_MULOP},
        {"/", MATH_DIVOP},
        {"==", LOGIC_JEOP},
        {"!=", LOGIC_JNEOP},
        {">=", LOGIC_JGEOP},
        {"<=", LOGIC_JNGOP},
        {">", LOGIC_JGOP},
        {"<", LOGIC_JGEOPS},
        {"(", LPAR},
        {")", RPAR},
        {"[", LBAR},
        {"]", RBAR},
        {"{", CLBAR},
        {"}", CRBAR},
        {"\"", QUOTE},
        {"'", QUOTE},
        {"=", EQL},
        {",", COMMA},
        {"\\", BSLASH},
        {";", NEWOP},
        {"EOF", BSEOF}
    };

    const static token bs_NewOp( void );

    class tokenizer
    {
        public:
            string_vector lines;
            tokenizer(string_vector fileLines);
            ~tokenizer() = default;

            bool isNumber(string str);
            bool isPunctuation(string str);
            bool isKeyword(string str);
            
            token_vector tokenizeLine(cstrref line);
            vector<token_vector> tokenize( void );
            

    };
}


#endif
