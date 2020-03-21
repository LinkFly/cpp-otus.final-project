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
	return call_test(__PRETTY_FUNCTION__, []() -> bool {
		DIBuilder diBuilder;
		//// Fixtures
		int n1 = 42;
		int n2 = 3;
		int waitRes = 45;
		//// end Fixtures

		shared_ptr<Number> num = diBuilder.create<Number>(n1);
		shared_ptr<Number> num2 = diBuilder.create<Number>(n2);


		PlusLispFunction plus;
		shared_ptr<ICallResult> res = diBuilder.createCallResult();
		shared_ptr<IEvaluator> evaluator = diBuilder.createEvaluator();
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*evaluator);
		plus.call(
			*ctx,
			ArgsList(num, num2),
			res);
		Number& numRes = res->getNumberResult();
		return numRes.getValue() == waitRes;
	});
}

bool simple_test() {
	bool res = call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;

		lisp.evalSexprStr(gstring{ "(plus 32 6)" });
		Number& numRes = lisp.getLastResult<Number>();
		return numRes.getValue() == 38;
	});
	/*LispEngine::clean();*/
	return res;
}

bool nested_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		/*lisp.readProgram(gstring{ "(plus (plus 30 2) 6)" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(plus (plus 30 2) 6)" });
		Number& numRes = lisp.getLastResult<Number>();
		bool res = numRes.getValue() == 38;
		/*lisp.clean();*/
		return res;
		});
}

bool cons_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;

		int64_t waitFirst = 42;
		int64_t waitSecond = 3;
		/*lisp.readProgram(gstring{ "(cons 42 3)" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(cons 42 3)" });
		Cons& consRes = lisp.getLastResult<Cons>();
		shared_ptr<Number> firstElt = std::static_pointer_cast<Number>(consRes.car());
		bool isFirst = firstElt->getValue() == waitFirst;
		shared_ptr<Number> secondElt = std::static_pointer_cast<Number>(consRes.cdr());
		bool isSecond = secondElt->getValue() == waitSecond;
		bool res = isFirst && isSecond;
		/*lisp.clean();*/
		return res;
		});
}

bool car_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		/*lisp.readProgram(gstring{ "(car (cons 42 3))" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(car (cons 42 3))" });
		Number& numRes = lisp.getLastResult<Number>();
		bool res = numRes.getValue() == waitRes;
		/*lisp.clean();*/
		return res;
		});
}

bool cdr_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 3;
		/*lisp.readProgram(gstring{ "(cdr (cons 42 3))" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(cdr (cons 42 3))" });
		Number& numRes = lisp.getLastResult<Number>();
		bool res = numRes.getValue() == waitRes;
		/*lisp.clean();*/
		return res;
		});
}

bool self_evaluated_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		/*lisp.readProgram(gstring{ "42" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "42" });
		Number& numRes = lisp.getLastResult<Number>();
		bool isCheckNum = numRes.getValue() == waitRes;

		PSexpr& lastRes = lisp.getLastPSexprRes();
		lisp.registerSymbolValue("test", lastRes);
		/*lisp.readProgram(gstring{ "test" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "test" });
		Number& numRes2 = lisp.getLastResult<Number>();
		bool isCheckSymVal = numRes2.getValue() == waitRes;

		/*lisp.readProgram(gstring{ "t" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "t" });
		Symbol& symT = lisp.getLastResult<Symbol>();
		bool isTrue = symT.getName() == gstring{ "t" };
		
		bool res = isCheckNum && isCheckSymVal && isTrue;
		/*lisp.clean();*/
		return res;
	});
}

bool setq_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitRes = 42;
		/*lisp.readProgram(gstring{ "(setq test 42)" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(setq test 42)" });

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
		lisp.evalSexprStr(gstring{ "(quote (42 3))" });

		Cons& cons = lisp.getLastResult<Cons>();
		Number* pNum = Number::AsNumber(cons.car()).get();
		auto num1 = pNum->getValue();
		Cons* pNextCons = Cons::AsCons(cons.cdr()).get();
		Number* pNum2 = Number::AsNumber(pNextCons->car()).get();
		auto num2 = pNum2->getValue();
		return num1 == waitNum1 && num2 == waitNum2;
	});
}

bool eval_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 45;
		lisp.evalSexprStr(gstring{ "(eval (quote (plus 42 3)))" });

		Number& numRes = lisp.getLastResult<Number>();
		return numRes.getValue() == waitNum;
		});
}

bool if_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		bool isSingleT, isSingleNil, isNestedT, isNestedNil;
		{
			int64_t waitNum1 = 1;
			int64_t waitNum2 = 2;
			/*lisp.readProgram(gstring{ "(if t 1 2)" });
			lisp.evalProgram();*/
			lisp.evalSexprStr(gstring{ "(if t 1 2)" });
			Number& num1 = lisp.getLastResult<Number>();
			isSingleT = num1.getValue() == waitNum1;

			/*lisp.readProgram(gstring{ "(if nil 1 2)" });
			lisp.evalProgram();*/
			lisp.evalSexprStr(gstring{ "(if nil 1 2)" });
			Number& num2 = lisp.getLastResult<Number>();
			isSingleNil = num2.getValue() == waitNum2;
		}

		{
			int64_t waitNum1 = 5;
			int64_t waitNum2 = 12;
			/*lisp.readProgram(gstring{ "(if t (plus 2 3) (plus 7 5))" });
			lisp.evalProgram();*/
			lisp.evalSexprStr(gstring{ "(if t (plus 2 3) (plus 7 5))" });
			Number& num1 = lisp.getLastResult<Number>();
			isNestedT = num1.getValue() == waitNum1;

			/*lisp.readProgram(gstring{ "(if nil (plus 2 3) (plus 7 5))" });
			lisp.evalProgram();*/
			lisp.evalSexprStr(gstring{ "(if nil (plus 2 3) (plus 7 5))" });
			Number& num2 = lisp.getLastResult<Number>();
			isNestedNil = num2.getValue() == waitNum2;
		}

		/*return isSingleT && isSingleNil;*/
		return isSingleT && isSingleNil && isNestedT && isNestedNil;
		});
}

bool let_trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		/*lisp.readProgram(gstring{ "(let () (plus 2 5) (plus 40 2))" });
		lisp.evalProgram();*/
		lisp.evalSexprStr(gstring{ "(let () (plus 2 5) (plus 40 2))" });
		return waitNum == lisp.getLastResult<Number>().getValue();
	});
}

bool let_simple_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{ "(let ((x 2)) (plus 2 5) (plus 40 x))" });
		/*lisp.evalSexprStr(gstring{ "(let ((x 2)) 42)" });*/
		return waitNum == lisp.getLastResult<Number>().getValue();
		});
}

bool lambda_trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{ "(lambda (x y) (plus x y))" });
		auto strRes = lisp.princ(lisp.getLastPSexprRes());
		return strRes == "<function>";
	});
}

bool apply_trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{ "(apply (quote plus) (cons 40 (cons 2 nil)))" });
		return waitNum == lisp.getLastResult<Number>().getValue();
		});
}

bool apply_lambda_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{ "(apply (lambda (x y) (plus x y)) (cons 40 (cons 2 nil)))" });
		auto res = lisp.getLastResult<Number>();
		return waitNum == res.getValue();
		});
}

//C:\learn-build\project-work\LispEngine\tests\fixtures\for-load-trivial.lisp
bool load_trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{ "(load \"..\\tests\\fixtures\\for-load-trivial.lisp\")" });
		auto res = lisp.getLastResult<Number>();
		return waitNum == res.getValue();
		});
}

bool other_arithm_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 42;
		lisp.evalSexprStr(gstring{
"(- 50 "
"	(*"
"		(/ 16 (+ 3 1))"
"		2)"
")"
			});
		auto res = lisp.getLastResult<Number>();
		return waitNum == res.getValue();
		});
}

bool predicates_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		

		auto testPredOnTrue = [&lisp](gstring& sexprStr) {
			gstring waitTrue = "t";
			/*lisp.evalSexprStr(gstring{ "(symbolp (quote some))" });*/
			lisp.evalSexprStr(sexprStr);
			auto lastRes = lisp.getLastPSexprRes();
			auto resIsSym = std::static_pointer_cast<Symbol>(lastRes);
			bool isSym = waitTrue == resIsSym->getName();
			return isSym;
		};

		auto testPredOnFalse = [&lisp](gstring& sexprStr) {
			lisp.evalSexprStr(sexprStr);
			auto lastRes = lisp.getLastPSexprRes();
			return lastRes->isNil();
		};
		
		bool isSym = testPredOnTrue(gstring{ "(symbolp (quote some))" });
		bool isNotSym = testPredOnFalse(gstring{ "(symbolp 42)" });

		bool isAtom = testPredOnTrue(gstring{ "(atomp t)" });
		bool isNotAtom = testPredOnFalse(gstring{ "(atomp (cons t nil))" });

		bool isCons = testPredOnTrue(gstring{ "(consp (cons t nil))" });
		bool isNotCons = testPredOnFalse(gstring{ "(consp t)" });

		//symbolp, atomp, consp, numberp, functionp, stringp
		bool isNum = testPredOnTrue(gstring{ "(numberp 42)" });
		bool isNotNum = testPredOnFalse(gstring{ "(numberp (car (cons t nil)))" });

		bool isFunc = testPredOnTrue(gstring{ "(functionp (lambda (x y) (plus x y)))" });
		bool isNotFunc = testPredOnFalse(gstring{ "(functionp (plus 3 4))" });

		bool isString = testPredOnTrue(gstring{ "(stringp (car (cons \"this-str\" t)))" });
		bool isNotString = testPredOnFalse(gstring{ "(stringp (lambda (x) x))" });


		return isSym && isNotSym
			&& isAtom && isNotAtom
			&& isCons && isNotCons
			&& isNum && isNotNum
			&& isFunc && isNotFunc
			&& isString && isNotString
			;
		});
}

bool tagbody_trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		LispEngine lisp;
		int64_t waitNum = 15;
		lisp.evalSexprStr(gstring{
"(let ((val nil))"
"	(tagbody"
"	  (setq val 1)"
"	  (go point-a)"
"	  (setq val (+ val 16))"
"	 point-c"
"	  (setq val (+ val 4))"
"	  (go point-b)"
"	  (setq val (+ val 32))"
"	 point-a"
"	  (setq val (+ val 2))"
"	  (go point-c)"
"	  (setq val (+ val 64))"
"	 point-b"
"	  (setq val (+ val 8)))"
"	val)"
			});
		auto res = lisp.getLastPSexprRes();
		auto numRes = static_cast<Number*>(res.get())->getValue();
		return waitNum == numRes;

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
	BOOST_CHECK(eval_test());
	BOOST_CHECK(if_test());
	BOOST_CHECK(let_trivial_test());
	BOOST_CHECK(let_simple_test());
	BOOST_CHECK(lambda_trivial_test());
	BOOST_CHECK(apply_trivial_test());
	BOOST_CHECK(apply_lambda_test());
	BOOST_CHECK(load_trivial_test());
	BOOST_CHECK(other_arithm_test());
	BOOST_CHECK(predicates_test());
	BOOST_CHECK(tagbody_trivial_test());
}
BOOST_AUTO_TEST_SUITE_END()
