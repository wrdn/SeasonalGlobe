#include "LSystem.h"
#include "PerfTimer.h"

#include <iostream>
using namespace std;

int main()
{
	LSystem _lsys;
	_lsys.AddAxiom('F', "GF");
	_lsys.AddAxiom('G', "FG");
	_lsys.SetStartingAxiom("F");
	
	PerfTimer pt;
	pt.start();
	_lsys.Evaluate(20);
	pt.end();
	cout << pt.time() << endl;

	getchar();

	return 0;
};