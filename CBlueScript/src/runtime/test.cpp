
#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <map>

#include "bs_types.h"
using namespace std;


int main ( void )
{
    BS_NumType testNum = BS_NumType();
    testNum.setValue("10");
    cout << testNum.isFloat() << endl;
}