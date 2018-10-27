#pragma once

// include
#include <vector>
#include "Term.h"

// defines
//#define DEBUG
#define INF 214748367

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
std::vector<Term> petricksMethod();
bool outputFile(char*);

// global variables
int varNum; // number of variables
int outputVarNum; // number of output variables, should be 1
int pNum; // number of p
int finalTermNum; // number of terms of the final answer
int literalNum; // number of literals of the final answer

// global variables
std::vector<std::string> varSymbols; // symbols of variables
std::vector<std::string> outputVarSymbols; // symbols of output variables whose length shoud be 1

// global variables
std::vector<Term> termList; // list of terms
std::vector<Term> tmpTermList; // used for terms combination

// global variables
struct Implicant {
	int number;
	bool isDontCare;

	Implicant() { number = 0; isDontCare = false; }
	Implicant(int newNumber, bool newIsDontCare) { number = newNumber; isDontCare = newIsDontCare; }
};
std::vector<Implicant> implicantList; // list of implicants

// global variables
std::vector<std::vector<Term> > mintermList; // list of minterms
std::vector<Term> finalAnswer;