#include "bs_tokenizer.h"

BS::tokenizer::tokenizer(string_vector fileLines)
{
    this->lines = fileLines;
}

const static token BS::bs_NewOp( void )
{
    token t;
    t.isCmd = true;
    t.ivalue = BS::NEWOP;
    return t;
}

bool BS::tokenizer::isNumber(string str)
{
    return str.find_first_not_of("0123456789") == string::npos;
}

bool BS::tokenizer::isPunctuation(string str)
{
    return str.find_first_not_of("<>!()[]{}\\;,\\.") == string::npos;
}

bool BS::tokenizer::isKeyword(string str)
{
    auto it = BS::BS_BINDS.find(str);
    if (it != BS::BS_BINDS.end())
        return true;
    
    return false;
}


token_vector BS::tokenizer::tokenizeLine(cstrref line)
{
    token_vector tokens;

    /*
        loop through the line by word and tokenize it
        and if there are quotes in the line
        then ignore until the end quote.
    */
    string word;
    bool inQuotes = false;
    for (int i = 0; i < line.size(); i++)
    {
        char c = line[i];
        
        if (c == '\"' || c == '\'')
        {
            if (!inQuotes) 
            {
                inQuotes = true;
                continue;
            }
            else
                inQuotes = false;
        }

        if (c == ' ' || c == ';' && !inQuotes )
        {
            // check the end of the word for ";"
            if (word.find_first_of(";") != string::npos)
                word = word.substr(0, word.find_first_of(";"));
            

            if (word.size() > 0)
            {
                token t;
                if (isNumber(word))
                {
                    t.isCmd = false;
                    t.svalue = word;
                    tokens.push_back(t);
                }
                else if (isPunctuation(word))
                {
                    t.isCmd = true;
                    t.ivalue = BS::BS_BINDS[word];
                    tokens.push_back(t);
                }
                else if (isKeyword(word))
                {
                    t.isCmd = true;
                    t.ivalue = BS::BS_BINDS[word];
                    tokens.push_back(t);
                }
                else
                {
                    t.isCmd = false;
                    t.svalue = word;
                    tokens.push_back(t);
                }
                word = "";
            }
        }

        else if (isPunctuation(string(1, c)) && !inQuotes)
        {
            token t;
            t.isCmd = true;
            t.ivalue = BS::BS_BINDS[string(1,c)];
            tokens.push_back(t);

            if (word.size() > 0)
            {
                token t;
                t.isCmd = false;
                t.svalue = word;
                tokens.push_back(t);
                word = "";
            }
        }

        else
        {
	    if (c != '"')
            	word += c;
        }
    }
    if (inQuotes)
    {
	    printf("Error: Unclosed quotes in line: %s\n", line.c_str());
	    exit(1);
    }
    tokens.push_back(bs_NewOp());
    return tokens;
}

vector<token_vector> BS::tokenizer::tokenize(void)
{
    vector<token_vector> tokens;
    for (int i = 0; i < this->lines.size(); i++)
    {
        tokens.push_back(tokenizeLine(this->lines[i]));
    }
    return tokens;
}
