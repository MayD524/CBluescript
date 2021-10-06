/*
    Author: May Draskovics
    Date: 9/17/2021
*/

#define BLUE_SCRIPT

#include <iostream>
#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"
#include "bs_runtime.h"
using namespace std;

vector<string> argsToVector( char ** args )
{
    vector<string> returnVal; 
    for ( int i = 0; i < sizeof(args); i++)
    {
        if (args[i] == NULL) { return returnVal; }
        returnVal.push_back(args[i]);
    }
    return returnVal;
}

int main( int argc, char** argv )
{
    vector<string> args = argsToVector(argv);
    bool debugEnabled = false;
    if (argc > 0)
    {
        string fname = string(argv[1]);
        //6cout << fname << endl;
        vector<string> fData = readFile(fname);

        if (anyInVector<string>("debug", args))
            debugEnabled = true;

        bs_runtime runtime = bs_runtime(fData, debugEnabled);

        runtime.runtimeMain();
    }
}