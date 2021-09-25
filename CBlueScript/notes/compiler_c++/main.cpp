#define DEBUG
#define BLUE_SCRIPT

#include <iostream>
#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"
#include "bs_parser.h"


using namespace std;

int main( void )
{
    string fileToRead = "test_file.bs";
    vector<string> fdata = readFile(fileToRead);
    bs_parser parser = bs_parser(fdata, fileToRead);

    parser.parseFile();

}