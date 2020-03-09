#include <iostream>
#include <memory>

#include "LispEngine.h"

// test
#include "test.h"
//#include "interfaces/IDIBuilder.h"


using std::cout;
using std::endl;

int main(int argc, char** argv) {
	//DIBuilder diBuilder;
	//shared_ptr<ITest> test = diBuilder->create<ITest>(3, 3.3);
	//test->doIt();

	LispEngine lisp;
	//lisp.readProgram(gstring{ "(plus 32 6)" });
	lisp.readProgram(gstring{ "(plus (plus 30 2) 6)" });
	lisp.evalProgram();
	Number& numRes = lisp.getLastResult<Number>();

	cout << numRes.getValue() << endl;
}