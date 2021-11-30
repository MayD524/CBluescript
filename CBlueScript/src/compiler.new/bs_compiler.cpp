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

void BS::Compile(cstrref fileName, cstrref outputFileName)
{
    string_vector lines = cleanComments(readFile(fileName));
    tokenizer tk = tokenizer(lines);
    vector<token_vector> tokens = tk.tokenize();

    for (int i = 0; i < tokens.size(); i++)
    {
        for (int j = 0; j < tokens[i].size(); j++)
        {
            token t = tokens[i][j];
            if (t.isCmd)
            {
                cout << t.ivalue << " ";
            }
            else
            {
                cout << t.svalue << " ";
            }
        }
        cout << endl;
    }
}
