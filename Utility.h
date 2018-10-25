#pragma once

// include
#include <vector>
#include "Term.h"

// define
#define DEBUG

// defines
#define CMD_I	".i"
#define CMD_O	".o"
#define CMD_ILB	".ilb"
#define CMD_OB	".ob"
#define CMD_P	".p"
#define CMD_E	".e"

// typedef
typedef unsigned int uint;

// function prototypes
bool getInput(int, char**);
void getTerms(int, int, std::string, const std::string, const std::string);
void quineMcClusky();
bool combineTerms();
bool petricksMethod();

// global variables
int varNum; // number of variables
int outputVarNum; // number of output variables, should be 1
int pNum; // number of p

// global variables
std::vector<std::string> varSymbols; // symbols of variables
std::vector<std::string> outputVarSymbols; // symbols of output variables whose length shoud be 1

// global variables
std::vector<Term> termList; // list of terms
std::vector<Term> tmpTermList; // used for terms combination

// global variables
struct Minterm {
	int number;
	bool isDontCare;

	Minterm() { number = 0; isDontCare = false; }
	Minterm(int newNumber, bool newIsDontCare) { number = newNumber; isDontCare = newIsDontCare; }
};
std::vector<Minterm> mintermList; // list of minterms