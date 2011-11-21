#include "LSystem.h"
#include <vector>
using namespace std;

LSystem::LSystem() : buildLevel(0) {
};

LSystem::~LSystem() {
};

bool LSystem::HasAxiom(const char c) const {
    return productionRules.count(c) > 0;
};

bool LSystem::AddAxiom(char c, string str) {
    if (HasAxiom(c)) return false;

    productionRules[c] = str;
    return true;
};

const string& LSystem::GetReplacementString(const char axiom) {
	if(_last_axiom_request == axiom) return *_last_axiom_result;

	_last_axiom_request = axiom;
	_last_axiom_result = &productionRules.at(axiom);
	return *_last_axiom_result;
};

void LSystem::ClearProductionRules() {
    productionRules.clear();
};

const string& LSystem::GetStartingAxiom() const {
    return startingAxiom;
};

void LSystem::SetStartingAxiom(string startAxiom) {
    startingAxiom = startAxiom;
};

const unsigned int LSystem::GetLSystemBuildLevel() const {
    return buildLevel;
};

const string LSystem::GetEvaluatedString() const {
    return evaluatedString;
};


// Rebuild LSystem string 'level' times using the ProductionRules

const string LSystem::Evaluate(const unsigned int level) {
    if (!startingAxiom.length()) return string();
    if (!level) return startingAxiom; // At level 0, we have the starting axiom

	buildLevel = level;
    evaluatedString = startingAxiom;

	string replacementString;
	replacementString.reserve(1000000); // stops too many reallocations

	evaluatedString.reserve(1000000);
	evaluatedString = startingAxiom;

	for(unsigned int i=1;i<=buildLevel;++i)
	{
		replacementString.clear();

		for (unsigned int j = 0; j < evaluatedString.length(); ++j)
		{
			char _c = evaluatedString[j];

			if(_last_axiom_request == _c)
				replacementString.append(*_last_axiom_result);
			else if(HasAxiom(_c))
			{
				_last_axiom_request = _c;
				_last_axiom_result = &productionRules[_c];
				replacementString.append(*_last_axiom_result);
			}
			else
			{
				replacementString += _c; // constant (not a variable)
			}
		}

		evaluatedString = replacementString;
	}

    return evaluatedString;
};

const string LSystem::Evaluate(const string startAxiom, const unsigned int level) {
    SetStartingAxiom(startAxiom);
    return Evaluate(level);
};