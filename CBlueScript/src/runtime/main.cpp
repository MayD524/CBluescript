/*
    Author: May Draskovics
    Date: 9/17/2021
*/

#define BLUE_SCRIPT
//#define DEBUG  // for enabling debug
//#define BLUE_SCRIPT_STEPTHROUGH // enable step through
//#define BS_AUTOWALK
#include <iostream>
#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"
#include "bs_runtime.h"
using namespace std;

int main( int argc, char** argv )
{
    if (argc > 0)
    {
        string fname = string(argv[1]);
        cout << fname << endl;
        vector<string> fData = readFile(fname);

        bs_runtime runtime = bs_runtime(fData);

        runtime.runtimeMain();
    }
}