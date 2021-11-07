#include <iostream>
#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"

using namespace std;

int main( int argc, char** argv )
{
    vector<string> args = charArrayToVector(argv);

    int ret = system("python3.10 --version");
    cout << ret << endl;

    if ( argc > 0 )
    {
        
    }
}
