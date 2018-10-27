#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
#include "Utility.h"

int main(int argc, char** argv) {
	if (!getInput(argc, argv))
		return 0;

	quineMcClusky();

	finalAnswer = petricksMethod();

	#ifdef DEBUG
	puts("in main");
	for (int j = 0; j < finalAnswer.size(); j++)
		std::cout << finalAnswer[j].getBinaryRep() << "|";
	puts("");
	#endif

	outputFile(argv[2]);

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
			implicantList.clear();
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

			if (output == "1" || output == "-")
				getTerms(input.length() - 1, 0, "", input, output);

			varCount++;
		}
	}

	sort(termList.begin(), termList.end(), cmpTerm);
	sort(implicantList.begin(), implicantList.end(), Implicant::cmpImplicant);

	#ifdef DEBUGS
	for (int k = 0; k < termList.size(); k++) {
		for (std::vector<int>::iterator it_v = termList[k].getM().begin(); it_v != termList[k].getM().end(); it_v++)
			std::cout << (*it_v) << ", ";

		std::cout << termList[k].getBinaryRep() << "|" << std::endl;
	}
	#endif

	fin.close();

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

		Implicant imp(curNum, (output == "-"));
		implicantList.push_back(imp);

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
					if (diffIdx == -1)
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

std::vector<Term> petricksMethod() {
	std::vector<std::vector<Term> > equation, tmpEquation;
	std::vector<std::vector<std::vector<Term> > > equas, tmpEquas;
	std::vector<Term> sum, product, ret;
	bool found;
	int foundNum;
	int minSize = INF, minLiteralNum = INF;

	// create petrick
	for (std::vector<Implicant>::iterator it_v = implicantList.begin(); it_v != implicantList.end(); it_v++) {
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

			if (found)
				sum.push_back(termList[k]);
		}

		equation.push_back(sum);
	}

	#ifdef DEBUG
	puts("\nin Petrick");
	for (int k = 0; k < equation.size(); k++) {
		std::cout << "(";
		for (int j = 0; j < equation[k].size(); j++)
			std::cout << equation[k][j].getBinaryRep() << (j == equation[k].size() - 1 ? "" : " + ");
		std::cout << ")";
	}
	puts("");
	#endif

	// simplify: step 1
	for (int k = 0; k < equation.size(); k++) {
		for (int j = 0; j < equation.size(); j++) {
			if (k == j || equation[k].size() > equation[j].size())
				continue;

			foundNum = 0;
			found = false;

			for (std::vector<Term>::iterator it_vk = equation[k].begin(); it_vk != equation[k].end(); it_vk++) {
				for (std::vector<Term>::iterator it_vj = equation[j].begin(); it_vj != equation[j].end(); it_vj++) {
					if ((*it_vk) == (*it_vj)) {
						foundNum++;
						break;
					}
				}
			}

			// J is included K, e.g. (A)(A + B) or (B + C)(B + C + F)
			if (foundNum == equation[k].size()) {

				tmpEquation.clear();
				for (int i = 0; i < equation.size(); i++) {
					if (i == j)
						continue;

					tmpEquation.push_back(equation[i]);
				}

				equation.clear();
				for (int i = 0; i < tmpEquation.size(); i++)
					equation.push_back(tmpEquation[i]);

				k = -1;
				break;
			}
		}
	}

	#ifdef DEBUG
	puts("\nin Petrick");
	for (int k = 0; k < equation.size(); k++) {
		std::cout << "(";
		for (int j = 0; j < equation[k].size(); j++)
			std::cout << equation[k][j].getBinaryRep() << (j == equation[k].size() - 1 ? "" : " + ");
		std::cout << ")";
	}
	puts("");
	#endif

	// simplify: step 2
	equas.clear();
	for (int k = 0; k < equation.size(); k++) {

		tmpEquation.clear();
		for (int j = 0; j < equation[k].size(); j++) {

			std::vector<Term> ts;
			ts.push_back(equation[k][j]);
			tmpEquation.push_back(ts);
		}

		equas.push_back(tmpEquation);
	}

	// simplify: step 3
	for (int k = 0; k < equas.size(); k++) {
		for (int j = k + 1; j < equas.size(); j++) {

			tmpEquation.clear();
			for (int ik = 0; ik < equas[k].size(); ik++) {
				for (int ij = 0; ij < equas[j].size(); ij++) {
					product = equas[k][ik] * equas[j][ij];

					#ifdef DEBUG
					for (std::vector<Term>::iterator it_v = product.begin(); it_v != product.end(); it_v++)
						std::cout << (*it_v).getBinaryRep() << "]";
					puts("");
					#endif

					tmpEquation.push_back(product);
				}
			}

			tmpEquas.clear();
			tmpEquas.push_back(tmpEquation);
			for (int i = j + 1; i < equas.size(); i++)
				tmpEquas.push_back(equas[i]);

			equas.clear();
			for (int i = 0; i < tmpEquas.size(); i++)
				equas.push_back(tmpEquas[i]);

			k = -1;
			break;
		}
	}

	#ifdef DEBUG
	puts("\nin Petrick");
	for (int k = 0; k < equas.size(); k++) {
		std::cout << "(";
		for (int j = 0; j < equas[k].size(); j++) {
			for (int i = 0; i < equas[k][j].size(); i++)
				std::cout << equas[k][j][i].getBinaryRep() << (i == equas[k][j].size() - 1 ? "" : "|");

			std::cout << (j == equas[k].size() - 1 ? "" : " + ");
		}
		std::cout << ")";
	}
	puts("");
	#endif

	// simplify: step 4
	{
		equation.clear();
		for (int k = 0; k < equas[0].size(); k++)
			equation.push_back(equas[0][k]);
	}

	// simplify: step 5
	for (int k = 0; k < equation.size(); k++) {
		for (int j = 0; j < equation.size(); j++) {
			if (k == j || equation[k].size() > equation[j].size())
				continue;

			foundNum = 0;
			found = false;

			for (std::vector<Term>::iterator it_vk = equation[k].begin(); it_vk != equation[k].end(); it_vk++) {
				for (std::vector<Term>::iterator it_vj = equation[j].begin(); it_vj != equation[j].end(); it_vj++) {
					if ((*it_vk) == (*it_vj)) {
						foundNum++;
						break;
					}
				}
			}

			// J is included K, e.g. (A)(A + B) or (B + C)(B + C + F)
			if (foundNum == equation[k].size()) {

				tmpEquation.clear();
				for (int i = 0; i < equation.size(); i++) {
					if (i == j)
						continue;

					tmpEquation.push_back(equation[i]);
				}

				equation.clear();
				for (int i = 0; i < tmpEquation.size(); i++)
					equation.push_back(tmpEquation[i]);

				k = -1;
				break;
			}
		}
	}

	#ifdef DEBUG
	puts("\nin Petrick");
	for (int k = 0; k < equation.size(); k++) {
		for (int j = 0; j < equation[k].size(); j++)
			std::cout << equation[k][j].getBinaryRep() << (j == equation[k].size() - 1 ? "" : "|");
		std::cout << (k == equation.size() - 1 ? "" : " + ");
	}
	puts("");
	#endif

	// simplify: step 6
	{
		for (int k = 0; k < equation.size(); k++) {
			if (equation[k].size() < minSize)
				minSize = equation[k].size();
		}
		for (int k = 0; k < equation.size(); k++) {
			if (equation[k].size() == minSize)
				mintermList.push_back(equation[k]);
		}

		for (int k = 0; k < mintermList.size(); k++) {
			literalNum = 0;
			for (int j = 0; j < mintermList[k].size(); j++)
				literalNum += mintermList[k][j].countLiteralNum();

			if (literalNum < minLiteralNum)
				minLiteralNum = literalNum;
		}
		for (int k = 0; k < mintermList.size(); k++) {
			literalNum = 0;
			for (int j = 0; j < mintermList[k].size(); j++)
				literalNum += mintermList[k][j].countLiteralNum();

			if (literalNum == minLiteralNum) {

				ret.clear();
				for (int j = 0; j < mintermList[k].size(); j++)
					ret.push_back(mintermList[k][j]);

				#ifdef DEBUG
				std::cout << literalNum << ": ";
				for (int j = 0; j < mintermList[k].size(); j++)
					std::cout << mintermList[k][j].getBinaryRep() << "|";
				puts("");
				#endif

				break;
			}
		}
	}

	return ret;
}

bool outputFile(char* outputFileName) {
	finalTermNum = finalAnswer.size();

	std::cout << "Total number of terms: " << finalTermNum << "\n";
	std::cout << "Total number of literals: " << literalNum << "\n";

	std::ofstream fout;
	fout.open(outputFileName, std::ios::out);

	fout << ".i " << varNum << "\n.o 1\n.ilb ";
	for (std::vector<std::string>::iterator it_v = varSymbols.begin(); it_v != varSymbols.end(); it_v++)
		fout << (*it_v) << (it_v == varSymbols.end() - 1 ? "" : " ");
	fout << "\n.ob " << outputVarSymbols[0] << "\n";
	fout << ".p " << finalTermNum << "\n";
	for (int k = 0; k < finalTermNum; k++)
		fout << finalAnswer[k].getBinaryRep() << " 1\n";
	fout << ".e";

	fout.close();

	return true;
}