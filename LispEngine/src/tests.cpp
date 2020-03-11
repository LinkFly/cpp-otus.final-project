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
		shared_ptr<IEvaluator> evaluator = diBuilder.createEvaluator();
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*evaluator);
		plus.call(
			*ctx,
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

bool cons_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitFirst = 42;
		int64_t waitSecond = 3;
		lisp.readProgram(gstring{ "(cons 42 3)" });
		lisp.evalProgram();
		Cons& consRes = lisp.getLastResult<Cons>();
		shared_ptr<Number> firstElt = std::static_pointer_cast<Number>(consRes.car());
		bool isFirst = firstElt->getValue() == waitFirst;
		shared_ptr<Number> secondElt = std::static_pointer_cast<Number>(consRes.cdr());
		bool isSecond = secondElt->getValue() == waitSecond;
		return isFirst && isSecond;
		});
}

bool car_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		lisp.readProgram(gstring{ "(car (cons 42 3))" });
		lisp.evalProgram();
		Number& numRes = lisp.getLastResult<Number>();
		return  numRes.getValue() == waitRes;
		});
}

bool cdr_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 3;
		lisp.readProgram(gstring{ "(cdr (cons 42 3))" });
		lisp.evalProgram();
		Number& numRes = lisp.getLastResult<Number>();
		return  numRes.getValue() == waitRes;
		});
}

bool self_evaluated_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		lisp.readProgram(gstring{ "42" });
		lisp.evalProgram();
		Number& numRes = lisp.getLastResult<Number>();
		bool isCheckNum = numRes.getValue() == waitRes;

		PSexpr& lastRes = lisp.getLastPSexprRes();
		lisp.registerSymbolValue("test", lastRes);
		lisp.readProgram(gstring{ "test" });
		lisp.evalProgram();
		Number& numRes2 = lisp.getLastResult<Number>();
		bool isCheckSymVal = numRes2.getValue() == waitRes;
		
		return isCheckNum && isCheckSymVal;
	});
}

bool setq_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		lisp.readProgram(gstring{ "(setq test 42)" });
		lisp.evalProgram();

		Number& numRes = lisp.getLastResult<Number>();
		bool isCheckReturn = numRes.getValue() == waitRes;

		Sexpr* value = lisp.getSymbolValue("test").get();
		bool isWaitedSymValue = (*value)._getDType().tstruct.number.num == waitRes;

		return isCheckReturn && isWaitedSymValue;
		});
}

bool quote_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum1 = 42;
		int64_t waitNum2 = 3;
		lisp.readProgram(gstring{ "(quote (42 3))" });
		lisp.evalProgram();

		//Cons& cons = lisp.getLastResult<Cons>();
		//auto num1 = Number::AsNumber(cons.car)->getValue();
		//auto& pNextCons = Cons::AsCons(cons.cdr);
		//auto num2 = Number::AsNumber(pNextCons->car)->getValue();
		//return num1 == waitNum1 && num2 == waitNum2;
		return true;
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
	BOOST_CHECK(cons_test());
	BOOST_CHECK(car_test());
	BOOST_CHECK(cdr_test());
	BOOST_CHECK(self_evaluated_test());
	BOOST_CHECK(setq_test());
	BOOST_CHECK(quote_test());
}
BOOST_AUTO_TEST_SUITE_END()
