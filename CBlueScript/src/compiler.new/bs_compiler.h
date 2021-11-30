
#pragma once
#ifndef BS_COMPILER_H
#define BS_COMPILER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "UCPPL/UCPPL.h"
#include "bs_defines.h"
#include "bs_tokenizer.h"

using namespace std;


namespace BS
{
    string_vector cleanComments(string_vector lines);
    void Compile(cstrref fileName, cstrref outputFileName);
}



#endif