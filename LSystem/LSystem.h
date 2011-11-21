#ifndef __LSYS_H__
#define __LSYS_H__

#include <string>
#include <map>

// Variables: e.g. F, K
// Constants: Any variables that have not been added e.g. + -
// Starting Axiom: e.g. F
// Production Rules: Replacement strings for the variables e.g. F -> FF
// Example:
// Generation 0: F
// Generation 1: FF
// Generation 2: FFFF
// Generation 3: FFFFFFFF
class LSystem {
private:
    std::map<char, std::string> productionRules;
    std::string startingAxiom;

    std::string evaluatedString;
    unsigned int buildLevel;

	// speed up GetReplacementString by caching last result
	char _last_axiom_request;
	std::string* _last_axiom_result;
public:
    LSystem();
    ~LSystem();

    // fails if axiom 'c' already exists
    bool AddAxiom(char c, std::string str);

    // clears productionRules dictionary (essentially resetting the LSystem)
    void ClearProductionRules();

    // true if 'c' in productionRules
    bool HasAxiom(const char c) const;

    // returns the string from productionRules, if axiom 'c' exists
    const std::string& GetReplacementString(const char axiom);

    // returns startingAxiom
    const std::string& GetStartingAxiom() const;

    // sets startingAxiom to parameter startAxiom
    void SetStartingAxiom(std::string startAxiom);

    // build level is the 'level' parameter passed to Evaluate()
    const unsigned int GetLSystemBuildLevel() const;

    const std::string GetEvaluatedString() const;

    // Evaluate function is used to build the LSystem string, recursing 'level'
    // times. You may also set the starting axiom at Evaluation time if required
    // Setting the starting axiom in the Evaluate() function will call
    // SetStartingAxiom and set the starting axiom for the entire LSystem object
    const std::string Evaluate(const unsigned int level);
    const std::string Evaluate(const std::string startAxiom, const unsigned int level);
};

#endif