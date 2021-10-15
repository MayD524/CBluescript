#pragma once

#ifndef BS_ARRAYTOOLS_H
#define BS_ARRAYTOOLS_H

#include <vector>
#include <string>

#include "UCPPL/UCPPL.h"

#include "bs_memory.h"
#include "bs_defines.h"

using namespace std;

void bs_ARRAY_SIZE(bs_memory& mem, const string& cmd_args);
void bs_ARRAY_ERASE(bs_memory& mem, const string& cmd_args);
void bs_ARRAY_MAKE(bs_memory& mem, const string& cmd_args);
void bs_ARRAY_GET(bs_memory& mem, const string& cmd_args);
void bs_ARRAY_SET(bs_memory& mem, const string& cmd_args);

#endif