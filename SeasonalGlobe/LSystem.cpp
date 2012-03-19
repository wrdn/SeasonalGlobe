#include "LSystem.h"
#include <vector>
using namespace std;

LSystem::LSystem() : generations(0), _last_axiom_request(0), _last_axiom_result() {
};

LSystem::~LSystem() {
};

// returns true if axiom exists
bool LSystem::HasAxiom(const c8 c) const {
    return productionRules.count(c) > 0;
};

// adds axiom c with replacement string str
bool LSystem::AddAxiom(c8 c, const string &str) {
    if (HasAxiom(c)) return false;

    productionRules[c] = str;
    return true;
};

// gets replacement string given axiom
const string& LSystem::GetReplacementString(const c8 axiom) {
	if(_last_axiom_request == axiom) return *_last_axiom_result; // return last result if cached

	_last_axiom_request = axiom;
	_last_axiom_result = &productionRules.at(axiom); // cache axiom
	return *_last_axiom_result;
};

void LSystem::ClearProductionRules() {
    productionRules.clear();
};

const string& LSystem::GetStartingAxiom() const {
    return startingAxiom;
};

void LSystem::SetStartingAxiom(const string &startAxiom) {
    startingAxiom = startAxiom;
};

const u32 LSystem::GetLSystemGenerations() const {
    return generations;
};

const string& LSystem::GetEvaluatedString() const {
    return evaluatedString;
};

const std::string LSystem::Evaluate()
{
	return Evaluate(GetLSystemGenerations());
};

// Rebuild LSystem string 'level' times using the ProductionRules
const string LSystem::Evaluate(const u32 level) {
    if (!startingAxiom.length() || !level)
	{
		evaluatedString = startingAxiom;
		return evaluatedString; // At level 0, we have the starting axiom
	}

	generations = level;
    evaluatedString = startingAxiom;

	string replacementString;
	replacementString.reserve(1000000); // stops too many reallocations

	evaluatedString.reserve(1000000);
	evaluatedString = startingAxiom;

	// for each generation
	for(u32 i=1;i<=generations;++i)
	{
		replacementString.clear();

		// for every character in the evaluated string
		for (u32 j = 0; j < evaluatedString.length(); ++j)
		{
			c8 _c = evaluatedString[j];

			// replace if an axiom is found

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
				// otherwise add the character back in, as it is a constant value
				replacementString += _c; // constant (not a variable)
			}
		}

		evaluatedString = replacementString;
	}

    return evaluatedString;
};

const string LSystem::Evaluate(const string &startAxiom, const u32 level) {
    SetStartingAxiom(startAxiom);
    return Evaluate(level);
};

void LSystem::ConvertEvaluatedStringToUpperCase()
{
	for(u32 i=0;i<evaluatedString.length();++i)
	{
		evaluatedString[i] = (c8)toupper(evaluatedString[i]);
	}
};

void LSystem::ConvertEvaluatedStringToLowerCase()
{
	for(u32 i=0;i<evaluatedString.length();++i)
	{
		evaluatedString[i] = (c8)tolower(evaluatedString[i]);
	}
};