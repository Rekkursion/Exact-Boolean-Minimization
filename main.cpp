#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
#include "Utility.h"

int main(int argc, char** argv) {
	getInput(argc, argv);
	quineMcClusky();
	petricksMethod();

	return 0;
}

bool getInput(int argc, char** argv) {
	if (argc != 3) {
		puts("Wrong input");
		return false;
	}

	std::ifstream fin;
	fin.open(argv[1], std::ios::in);

	if (fin.fail()) {
		puts("PLA file not found");
		return false;
	}

	std::string word;
	int varCount = 0;

	while (fin >> word) {

		// .i
		if (word == CMD_I) {
			fin >> varNum;
			mintermList.clear();
		}

		// .o
		else if (word == CMD_O) {
			fin >> outputVarNum;
		}

		// .ilb
		else if (word == CMD_ILB) {
			varSymbols.clear();
			varSymbols.resize(varNum);

			for (int k = 0; k < varNum; k++)
				fin >> varSymbols[k];
		}

		// .ob
		else if (word == CMD_OB) {
			outputVarSymbols.clear();
			outputVarSymbols.resize(outputVarNum);

			for (int k = 0; k < outputVarNum; k++)
				fin >> outputVarSymbols[k];
		}

		// .p
		else if (word == CMD_P) {
			fin >> pNum;
			termList.clear();
		}

		// .e
		else if (word == CMD_E) {
			// end of input
			break;
		}

		// 10-1, 0-1-, etc
		else {
			std::string input = word, output;
			fin >> output;

			if(output == "1" || output == "-")
				getTerms(input.length() - 1, 0, "", input, output);

			varCount++;
		}
	}

	sort(termList.begin(), termList.end(), [] (Term& lhs, Term& rhs) -> bool { return (lhs.getM()[0] < rhs.getM()[0]); });
	sort(mintermList.begin(), mintermList.end(), [] (Minterm& lhs, Minterm& rhs) -> bool { return (lhs.number < rhs.number); });

	#ifdef DEBUGS
	for (int k = 0; k < termList.size(); k++) {
		for (std::vector<int>::iterator it_v = termList[k].getM().begin(); it_v != termList[k].getM().end(); it_v++)
			std::cout << (*it_v) << ", ";

		std::cout << termList[k].getBinaryRep() << "|" << std::endl;
	}
	#endif

	return true;
}

void getTerms(int curIdx, int curNum, std::string bRep, std::string input, const std::string output) {

	// exit
	if (curIdx == -1) {
		for (std::vector<Term>::iterator it_v = termList.begin(); it_v != termList.end(); it_v++) {
			if (it_v->getM()[0] == curNum)
				return;
		}

		std::vector<int> vec;
		vec.push_back(curNum);

		std::string trueBinaryRep = "";
		for (int k = bRep.length() - 1; k >= 0; k--)
			trueBinaryRep += bRep[k];

		Term t(vec, trueBinaryRep);
		termList.push_back(t);

		Minterm m(curNum, (output == "-"));
		mintermList.push_back(m);

		return;
	}


	if (input[curIdx] == '0') {
		getTerms(curIdx - 1, curNum, bRep + "0", input, output);
	}

	else if (input[curIdx] == '1') {
		getTerms(curIdx - 1, curNum + (1 << (input.length() - curIdx - 1)), bRep + "1", input, output);
	}

	else if (input[curIdx] == '-') {
		getTerms(curIdx - 1, curNum, bRep + "0", input, output);
		getTerms(curIdx - 1, curNum + (1 << (input.length() - curIdx - 1)), bRep + "1", input, output);
	}

	return;
}

void quineMcClusky() {
	while (combineTerms());
	return;
}

bool combineTerms() {
	static int count = 0;
	count++;

	if (count == 5000)
		return false;

	if (termList.size() == 0)
		return false;

	int size = termList.size(), diffIdx;
	std::string rep1, rep2;
	bool canCombine;

	tmpTermList.clear();

	for (int k = 0; k < size; k++) {
		for (int j = 0; j < size; j++) {
			if (k == j || termList[k].getBinaryRep().length() != termList[j].getBinaryRep().length())
				continue;

			rep1 = termList[k].getBinaryRep();
			rep2 = termList[j].getBinaryRep();
			diffIdx = -1;
			canCombine = true;

			for (int i = 0; i < rep1.length(); i++) {
				if ((rep1[i] == '-' && rep2[i] != '-') || (rep1[i] != '-' && rep2[i] == '-')) {
					canCombine = false;
					break;
				}

				if (rep1[i] != rep2[i]) {
					if(diffIdx == -1)
						diffIdx = i;
					else {
						canCombine = false;
						break;
					}
				}
			}

			if (canCombine) {
				std::set<int> minterms;
				std::vector<int> mintermList;
				std::string bRep = "";

				for (int i = 0; i < termList[k].getM().size(); i++)
					minterms.insert(termList[k].getM()[i]);
				for (int i = 0; i < termList[j].getM().size(); i++)
					minterms.insert(termList[j].getM()[i]);

				for (std::set<int>::iterator it_s = minterms.begin(); it_s != minterms.end(); it_s++)
					mintermList.push_back(*it_s);

				for (int i = 0; i < termList[k].getBinaryRep().length(); i++) {
					if (i == diffIdx)
						bRep += "-";
					else
						bRep += termList[k].getBinaryRep()[i];
				}

				bool already = false;
				for (int h = 0; h < tmpTermList.size(); h++) {
					if (tmpTermList[h].getBinaryRep() == bRep) {
						already = true;
						break;
					}
				}

				if (!already) {
					Term t(mintermList, bRep);
					tmpTermList.push_back(t);

					termList[k].hasBeenCombined = termList[j].hasBeenCombined = true;
				}
			}
		}
	}

	if (tmpTermList.size() == 0)
		return false;

	for (int i = 0; i < termList.size(); i++) {
		if (termList[i].hasBeenCombined == false)
			tmpTermList.push_back(termList[i]);
	}

	#ifdef DEBUG
	puts("==============================");
	for (int i = 0; i < tmpTermList.size(); i++) {
		std::cout << tmpTermList[i];
	}
	#endif

	termList.clear();
	for (int i = 0; i < tmpTermList.size(); i++)
		termList.push_back(tmpTermList[i]);

	return true;
}

bool petricksMethod() {
	std::vector<std::vector<std::vector<Term> > > equation;
	std::vector<std::vector<Term> > sum;

	for (std::vector<Minterm>::iterator it_v = mintermList.begin(); it_v != mintermList.end(); it_v++) {
		if (it_v->isDontCare)
			continue;

		sum.clear();
		for (int k = 0; k < termList.size(); k++) {

			bool found = false;
			for (int j = 0; j < termList[k].getM().size(); j++) {
				if (termList[k].getM()[j] == it_v->number) {
					found = true;
					break;
				}
			}

			if (found) {
				std::vector<Term> terms;
				terms.push_back(termList[k]);

				sum.push_back(terms);
			}
		}

		equation.push_back(sum);
	}

	#ifdef DEBUG
	puts("\nin Petrick");
	for (int k = 0; k < equation.size(); k++) {
		std::cout << "(";
		for (int j = 0; j < equation[k].size(); j++) {
			for(int i = 0; i < equation[k][j].size(); i++)
				std::cout << equation[k][j][0].getBinaryRep() << (i == equation[k][j].size() - 1 ? "" : "|");

			std::cout << (j == equation[k].size() - 1 ? "" : " + ");
		}
		std::cout << ")";
	}
	puts("");
	#endif

	for (int k = 0; k < equation.size(); k++) {
		for (int j = k + 1; j < equation.size(); j++) {

		}
	}

	return true;
}