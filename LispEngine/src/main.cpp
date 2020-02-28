#include <iostream>
#include <memory>

#include "LispEngine.h"

// test
#include "test.h"
//#include "interfaces/IDIBuilder.h"
#include "implements/DiBuilder.h"
#include "interfaces/DataStructs.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
	//std::unique_ptr<DIBuilder> diBuilder = std::make_unique<DIBuilder>();
	DIBuilder diBuilder;
	/*shared_ptr<ITest> test = diBuilder->create<ITest>(3, 3.3);
	test->doIt();*/

	shared_ptr<INumber> num = diBuilder.create<INumber>(42);
	cout << num->getNumber() << endl;
	return 0;
}