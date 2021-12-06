/*
    Author: May Draskovics
    Date:   11/28/2021
    Description:
        This is the main file for the compiler.
*/


#include <iostream>
#include <string>
#include <vector>

#include "bs_compiler.h"
#include "argparse.hpp"

using namespace std;

int main(int argc, char** argv)
{
    argparse::ArgumentParser parser("BlueScript Compiler");

    parser.add_argument("-o", "--out_file")
          .help("The output file name")
          .default_value("a.cbs");
    parser.add_argument("-f","--file")
          .help("The input file name")
          .required();

    parser.parse_args(argc, argv);


    string inpFile = parser.get<string>("file");
    string outFile;
    // just to avoid a weird bug with the argparse library
    // just anycast being stupid
    try{
        outFile = parser.get<string>("out_file");
    } catch (const bad_any_cast& e) {
        outFile = "a.cbs";
    }
    
    string_vector comp_lines = BS::Compile(inpFile, outFile);
    comp_lines.insert(comp_lines.begin(), "jmp main");
    for (cstrref line : comp_lines) {
	    cout << line << endl;
    }

    // create the output file and write the compiled code to it
    ofstream outFileHandler(outFile);
    for (cstrref line : comp_lines) {
        outFileHandler << line << endl;
    }
    outFileHandler.close();
}


