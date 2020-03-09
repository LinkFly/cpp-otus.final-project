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

	//PSexpr sym1 = lisp.parseSymbol("plus");
	
	//cout << "plusLispFunction(parse sym1): " << lisp._getFnFromSym(sym1) << endl;
	//PlusLispFunction& plusFn = *((PlusLispFunction*)(lisp._getFnFromSym(sym1)));
	//cout << "plusLispFunction(sym1 as PlusLispFunction): " << &plusFn << endl;
	lisp.readProgram(gstring{ "(plus (plus 30 2) 6)" });
	//PSexpr sym2 = lisp.parseSymbol("plus");
	//cout << "plusLispFunction(parse sym2): " << lisp._getFnFromSym(sym1) << endl;
	lisp.evalProgram();
	Number& numRes = lisp.getLastResult<Number>();

	cout << numRes.getValue() << endl;
}