#include "Term.h"



Term::Term() {
	m.clear();
	binaryRep = "";
}

Term::Term(std::vector<int> newM, std::string newBinaryRep) {
	setM(newM);
	setBinaryRep(newBinaryRep);
	hasBeenCombined = false;
}

Term::~Term() {}

std::ostream& operator<<(std::ostream& ostream, Term& term) {
	for (std::vector<int>::iterator it_v = term.getM().begin(); it_v != term.getM().end(); it_v++)
		ostream << (*it_v) << ", ";
	ostream << "|" << term.getBinaryRep() << "|" << term.getSymbol() << "\n";

	return ostream;
}

bool Term::operator==(Term& rhs) {
	if ((rhs.getBinaryRep() != this->getBinaryRep()) || (rhs.getM().size() != this->getM().size()))
		return false;

	for (int k = 0; k < rhs.getM().size(); k++) {
		if (rhs.getM()[k] != this->getM()[k])
			return false;
	}

	return true;
}

bool Term::operator!=(Term& rhs) {
	return (!(rhs == *this));
}

std::vector<Term> operator*(Term& lhs, Term& rhs) {
	std::vector<Term> lhsv, rhsv;
	lhsv.push_back(lhs);
	rhsv.push_back(rhs);

	return (lhsv * rhsv);
}

std::vector<Term> operator*(std::vector<Term>& lhs, std::vector<Term>& rhs) {
	std::vector<Term> ret;
	ret.clear();
	bool already;

	for (int k = 0; k < lhs.size(); k++) {
		already = false;

		for (int j = 0; j < ret.size(); j++) {
			if (lhs[k].getBinaryRep() == ret[j].getBinaryRep()) {

				already = true;
				break;
			}
		}

		if (!already) {
			ret.push_back(lhs[k]);
		}
	}

	for (int k = 0; k < rhs.size(); k++) {
		already = false;

		for (int j = 0; j < ret.size(); j++) {
			if (rhs[k].getBinaryRep() == ret[j].getBinaryRep()) {

				already = true;
				break;
			}
		}

		if (!already) {
			ret.push_back(rhs[k]);
		}
	}

	return ret;
}

bool Term::operator<(Term& rhs) {
	return (this->getM()[0] < rhs.getM()[0]);
}

std::vector<int>& Term::getM() {
	return m;
}

void Term::setM(std::vector<int> newM) {
	m.clear();
	m.resize(newM.size());

	for (int k = 0; k < newM.size(); k++)
		m[k] = newM[k];
}

std::string Term::getBinaryRep() {
	return binaryRep;
}
void Term::setBinaryRep(std::string newBinaryRep) {
	binaryRep = newBinaryRep;
}

std::string Term::getSymbol() {
	return symbol;
}

void Term::setSymbol(std::string newSymbol) {
	symbol = newSymbol;
}

int Term::countLiteralNum() {
	int ret = 0;

	for (int k = 0; k < binaryRep.length(); k++)
		ret += (binaryRep[k] != '-');

	return ret;
}