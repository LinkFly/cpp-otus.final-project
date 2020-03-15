#pragma once

#include <map>
#include <variant>
#include <memory>

//#include <boost/di.hpp>

#include "../share.h"
#include "../interfaces/IDIBuilder.h"

#include "../interfaces/i-data-structs.h"
#include "../interfaces/evaluator/IScope.h"
#include "../interfaces/evaluator/IReader.h"
#include "../interfaces/evaluator/IProgram.h"
#include "../interfaces/evaluator/IEvaluator.h"
#include "../interfaces/evaluator/IRunContext.h"
#include "../interfaces/evaluator/ICallResult.h"

#include "../interfaces/ILispEngine.h"

#include "../implements/data-structs.h"
#include "../implements/evaluator/Program.h"
#include "../implements/evaluator/Scope.h"
#include "../implements/evaluator/Reader.h"
#include "../implements/evaluator/Program.h"
#include "../implements/evaluator/Evaluator.h"
#include "../implements/evaluator/RunContext.h"
#include "../implements/evaluator/CallResult.h"

#include "../BaseFunctions.h"
#include "../Printer.h"
#include "../repl.h"

// test
#include "../test.h"

//namespace di = boost::di;
using std::map;
using std::shared_ptr;
using std::make_shared;

class DIBuilder : public IDIBuilder, public CClass {
	//enum class EOpType {
	//	bind, create
	//};
	//map<Cid, 
public:

	DIBuilder() = default;

	template<class Interface>
	Interface& get() {

	}

	template<class Implement, typename ...Args>
	shared_ptr<Implement> create(Args... args) {
		// TODO Create error function
		cerr << "ERROR: Bad creating\n";
		exit(-1);
	}

	template<>
	shared_ptr<ITest> create<ITest>(int i, double d) {
		//ITest* ptr = new CTest(i, d);
		//return shared_ptr<ITest>(ptr);
		return make_shared<CTest>(i, d);
	}

	template<>
	shared_ptr<Number> create<Number>(int32_t num) {
		return shared_ptr<Number>(new Number(num));
	}

	shared_ptr<Number> createNumber(int32_t num) override {
		return shared_ptr<Number>(new Number(num));
	}

	template<class SexprChild1, class SexprChild2>
	shared_ptr<Cons> createCons(SexprChild1& sexpr1OrChild, SexprChild2& sexpr2OrChild) {
		auto sexpr1 = std::static_pointer_cast<Sexpr>(sexpr1OrChild);
		auto sexpr2 = std::static_pointer_cast<Sexpr>(sexpr2OrChild);
		return shared_ptr<Cons>(new Cons(sexpr1, sexpr2));
	}

	shared_ptr<Nil> createNil() {
		return make_shared<Nil>();
	}

	// TODO!!! Don't work (why???)
	//template<>
	//shared_ptr<Symbol> create<Symbol>(gstring& symName) {
	//	return shared_ptr<Symbol>(new Symbol(symName));
	//}

	shared_ptr<Symbol> createSymbol(const gstring& symName) {
		return shared_ptr<Symbol>(new Symbol(symName));
	}

	template<>
	shared_ptr<SetfSymbolFunction> create<SetfSymbolFunction>() {
		return make_shared<SetfSymbolFunction>();
	}

	shared_ptr<Function> createFunction(shared_ptr<ILispFunction>& lispFunc) {
		return make_shared<Function>(lispFunc);
	}
	
	shared_ptr<IScope> createScope() override {
		return std::dynamic_pointer_cast<IScope>(make_shared<Scope>());
	}

	shared_ptr<IProgramContext> createProgramContext() override {
		return std::dynamic_pointer_cast<IProgramContext>(make_shared<ProgramContext>());
	}

	shared_ptr<IMemoryManager> createMemoryManager() override {
		return std::dynamic_pointer_cast<IMemoryManager>(make_shared<MemoryManager>());
	}

	shared_ptr<IReader> createReader(IDIBuilder& idiBuilder) {
		return std::dynamic_pointer_cast<IReader>(make_shared<Reader>(idiBuilder));
	}

	virtual shared_ptr<IProgram> createProgram(IRunContext& ctx/*IDIBuilder& idiBuilder*/) override {
		return std::dynamic_pointer_cast<IProgram>(make_shared<Program>(*this, ctx));
	}

	shared_ptr<IEvaluator> createEvaluator() {
		return std::dynamic_pointer_cast<IEvaluator>(make_shared<Evaluator>(*this));
	}

	virtual shared_ptr<IRunContext> createRunContext(IEvaluator& evaluator) override {
		auto res = std::dynamic_pointer_cast<IRunContext>(make_shared<RunContext>(evaluator, this));
		res->setDIBuilder(this);
		return res;
	}

	virtual shared_ptr<Printer> createPrinter() {
		return make_shared<Printer>();
		//return std::dynamic_pointer_cast<IRunContext>(make_shared<RunContext>(evaluator));
	}

	virtual shared_ptr<Repl> createRepl(ILispEngine& lispEngine) {
		return make_shared<Repl>(lispEngine);
		//return std::dynamic_pointer_cast<IRunContext>(make_shared<RunContext>(evaluator));
	}

	virtual shared_ptr<ICallResult> createCallResult() override {
		/*shared_ptr<ICallResult> ptr;
		return ptr;*/
		//return shared_ptr<ICallResult>();
		return std::dynamic_pointer_cast<ICallResult>(make_shared<CallResult>());
	}


	/*shared_ptr<ILispEngine> createLispEngine() {
		return std::dynamic_pointer_cast<ILispEngine>(make_shared<LispEngine>());
	}*/

	template<>
	shared_ptr<PlusLispFunction> create<PlusLispFunction>() {
		return make_shared<PlusLispFunction>();
	}

	template<>
	shared_ptr<CarLispFunction> create<CarLispFunction>() {
		return make_shared<CarLispFunction>();
	}

	template<>
	shared_ptr<CdrLispFunction> create<CdrLispFunction>() {
		return make_shared<CdrLispFunction>();
	}

	template<>
	shared_ptr<SetqLispFunction> create<SetqLispFunction>() {
		return make_shared<SetqLispFunction>();
	}


	template<>
	shared_ptr<ConsLispFunction> create<ConsLispFunction>() {
		return make_shared<ConsLispFunction>();
	}

	template<>
	shared_ptr<QuoteLispFunction> create<QuoteLispFunction>() {
		return make_shared<QuoteLispFunction>();
	}

	template<>
	shared_ptr<IfLispFunction> create<IfLispFunction>() {
		return make_shared<IfLispFunction>();
	}

	template<>
	shared_ptr<LetLispFunction> create<LetLispFunction>() {
		return make_shared<LetLispFunction>();
	}
};