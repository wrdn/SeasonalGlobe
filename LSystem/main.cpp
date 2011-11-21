#include "LSystem.h"
#include "PerfTimer.h"

#include <iostream>
using namespace std;

void PrintLSystemEvaluation(LSystem &lsys, unsigned int generation,
	string compare)
{
	string t = lsys.Evaluate(generation);
	cout << "Generation " << generation << ": " << t;

	if(compare == t)
		cout << "\t[Identical]" << endl;
	else
		cout << "\t[Different]" << endl;
};

int main()
{
	LSystem _lsys;

	// Wikipedia Example 2
	//_lsys.AddAxiom('0', "1[0]0");
	//_lsys.AddAxiom('1', "11");
	//_lsys.SetStartingAxiom("0");
	// _lsys.Evaluate(3);

	// Wikipedia Koch Curve
	//_lsys.AddAxiom('F', "F+F-F-F+F");
	//_lsys.SetStartingAxiom("F");
	//PrintLSystemEvaluation(_lsys, 0,"F");
	//PrintLSystemEvaluation(_lsys, 1,"F+F-F-F+F");
	//PrintLSystemEvaluation(_lsys, 2, "F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F");
	//PrintLSystemEvaluation(_lsys, 3, "F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F+F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F-F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F-F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F+F+F-F-F+F+F+F-F-F+F-F+F-F-F+F-F+F-F-F+F+F+F-F-F+F");

	//_lsys.AddAxiom('F', "G[+&F][-%F]GFF@");
	//_lsys.AddAxiom('G', "GG");
	//_lsys.SetStartingAxiom("F");
	//string s = _lsys.Evaluate(2);

	//PerfTimer pt;
	//pt.start();
	//_lsys.Evaluate(15);
	//pt.end();
	//cout << pt.time() << endl;

	_lsys.AddAxiom('a', "ab");
	_lsys.AddAxiom('b', "a");
	_lsys.SetStartingAxiom("b");
	_lsys.Evaluate(5);

	getchar();

	return 0;
};