#pragma once

#include <iostream>

using std::cout;
using std::endl;

class ITest {
public:
	virtual void doIt() = 0;
};

class CTest : public ITest {
public:
	CTest(int i, double d) {};
	void doIt() override {
		cout << "CTest.doIt()" << endl;
	}
};
