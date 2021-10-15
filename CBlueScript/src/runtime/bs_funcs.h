#pragma once


#ifndef BS_FUNCS_H
#define BS_FUNCS_H

#include <time.h>
#include <vector>
#include <string>


#include "UCPPL/UCPPL.h"

#include "bs_memory.h"
#include "bs_defines.h"

using namespace std;

void bs_MOV(bs_memory& mem, const string& cmd_args);
void bs_RINT(bs_memory& mem, const string& cmd_args);
void bs_REM(bs_memory& mem, const string& cmd_args);
void bs_GETTYPE(bs_memory& mem, const string& cmd_args);
void bs_INDEX(bs_memory& mem, const string& cmd_args);
void bs_SET(bs_memory& mem, const string& cmd_args);
void bs_SIZEOF(bs_memory& mem, const string& cmd_args);
void bs_READ(bs_memory& mem, const string& cmd_args);
void bs_WRITE(bs_memory& mem, const string& cmd_args);
//void bs_CMP(bs_memory& mem, const string& cmd_args);

#endif