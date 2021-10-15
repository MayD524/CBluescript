/*
    Author: May Draskovics
*/
#pragma once

#ifndef UCPPL_H
#define UCPPL_H

#include <iostream>
#include <vector>
#include <string>

#include "tools.h"

// cluter free debugs 
// if in debug mode will compile cout
// otherwise it ignores
#ifdef DEBUG
    #define debug(x) cout << x << endl;
#else
    #define debug(x)
#endif

// for conversion (char) -> (int)
typedef unsigned char uchar;

using namespace std;

#endif