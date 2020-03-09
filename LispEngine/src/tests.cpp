#include "share.h"

#define BOOST_TEST_MODULE lisp_engine_test_module

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <functional>
#include <ctime>
#include <sstream>
#include <map>

#include "LispEngine.h"

using std::string;
using std::cout;
using std::endl;
using std::clock;

const string relpathPrefix{ "../tests/fixtures" };

// Utils
bool call_test(string name, std::function<bool(void)> fntest) {
	cout << "------------------------------\n";
	cout << endl << name << ":\n";

	auto startTime = clock();
	bool res = fntest();
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	cout << "------------------------------\n";
	return res;
}
//// end Utils

bool trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		DIBuilder diBuilder;
		//// Fixtures
		int n1 = 42;
		int n2 = 3;
		int waitRes = 45;
		//// end Fixtures

		shared_ptr<Number> num = diBuilder.create<Number>(n1);
		shared_ptr<Number> num2 = diBuilder.create<Number>(n2);


		PlusLispFunction plus;
		CallResult res;
		plus.call(
			ArgsList(num, num2),
			res);
		Number& numRes = res.getResult<Number>();
		return numRes.getValue() == waitRes;
	});
}

bool simple_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		lisp.readProgram(gstring{ "(plus 32 6)" });
		lisp.evalProgram();
		Number& numRes = lisp.getLastResult<Number>();
		return  numRes.getValue() == 38;
	});
}

bool nested_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		lisp.readProgram(gstring{ "(plus (plus 30 2) 6)" });
		lisp.evalProgram();
		Number& numRes = lisp.getLastResult<Number>();
		return  numRes.getValue() == 38;
		});
}

void init_base_fixtures() {
	// Init code must be here

}

struct Init {
	Init(std::function<void()> init_func) {
		init_func();
	}
};
#define INIT(init_func) struct Init init(init_func);

BOOST_AUTO_TEST_SUITE(lisp_engine_test_suite)
INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_lisp_engine)
{
	BOOST_CHECK(trivial_test());
	BOOST_CHECK(simple_test());
	BOOST_CHECK(nested_test());

}
BOOST_AUTO_TEST_SUITE_END()
