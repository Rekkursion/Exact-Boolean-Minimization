#pragma once

#include <vector>
#include <string>
#include <iostream>

class Term {
public:
	Term();
	Term(std::vector<int>, std::string);
	~Term();

	friend std::ostream& operator<<(std::ostream&, Term&);
	bool operator==(Term&);
	bool operator!=(Term&);

	std::vector<int>& getM();
	void setM(std::vector<int>);

	std::string getBinaryRep();
	void setBinaryRep(std::string);

	bool hasBeenCombined;

private:
	std::vector<int> m; // integers in this term
	std::string binaryRep; // binary representation
};

