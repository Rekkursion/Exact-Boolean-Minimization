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
	ostream << "|" << term.getBinaryRep() << "|\n";

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