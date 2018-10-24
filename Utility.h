#pragma once

#include <vector>
#include "Term.h"

#define DEBUG

#define CMD_I	".i"
#define CMD_O	".o"
#define CMD_ILB	".ilb"
#define CMD_OB	".ob"
#define CMD_P	".p"
#define CMD_E	".e"

typedef unsigned int uint;

int varNum; // number of variables
int outputVarNum; // number of output variables, should be 1
int pNum; // number of p

std::vector<std::string> varSymbols; // symbols of variables
std::vector<std::string> outputVarSymbols; // symbols of output variables whose length shoud be 1

std::vector<Term> termList; // list of terms
std::vector<Term> tmpTermList; // for terms combination