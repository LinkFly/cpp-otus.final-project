#pragma once

#include <map>
#include <vector>
#include <variant>
#include <memory>

//#include <boost/di.hpp>

#include "../base/share.h"
#include "../interfaces/IDIBuilder.h"

#include "../interfaces/core/i-data-structs.h"
#include "../interfaces/core/evaluator/IScope.h"
#include "../interfaces/core/IReader.h"
#include "../interfaces/core/evaluator/IProgram.h"
#include "../interfaces/core/evaluator/IEvaluator.h"
#include "../interfaces/core/evaluator/IRunContext.h"
#include "../interfaces/core/evaluator/ICallResult.h"

#include "../interfaces/core/ILispEngine.h"

#include "../implements/core/data-structs.h"
#include "../implements/core/evaluator/Program.h"
#include "../implements/core/evaluator/Scope.h"
#include "../implements/core/Reader.h"
#include "../implements/core/evaluator/Program.h"
#include "../implements/core/evaluator/Evaluator.h"
#include "../implements/core/evaluator/RunContext.h"
#include "../implements/core/evaluator/CallResult.h"

#include "../implements/core/evaluator/BaseFunctions.h"
#include "../implements/core/Printer.h"
#include "../implements/core/Repl.h"

using std::map;
using std::vector;

class DIBuilder : public IDIBuilder, public CClass {
public:
	DIBuilder() = default;

	virtual shared_ptr<Number> createNumber(int64_t num) override {
		return create<Number>(num);
	}

	template<class SexprChild1, class SexprChild2>
	shared_ptr<Cons> createCons(SexprChild1& sexpr1OrChild, SexprChild2& sexpr2OrChild) {
		auto sexpr1 = std::static_pointer_cast<Sexpr>(sexpr1OrChild);
		auto sexpr2 = std::static_pointer_cast<Sexpr>(sexpr2OrChild);
		return create<Cons>(sexpr1, sexpr2);
	}

	shared_ptr<Nil> createNil() {
		static auto nil = create<Nil>();
		return nil;
	}

	virtual shared_ptr<IPackage> createPackage() override {
		return bind< IPackage, Package >(*this);
	}

	shared_ptr<Symbol> createSymbol(const gstring& symName) {
		return bind<Symbol>(symName);
	}

	virtual shared_ptr<String> createString(const gstring& str) override {
		return bind<String>(str);
	}

	shared_ptr<Function> createFunction(shared_ptr<ILispFunction>& lispFunc) {
		return create<Function>(lispFunc);
	}

	virtual shared_ptr<Lambda> createLambda(PSexpr params, PSexpr forms) override {
		return create<Lambda>(params, forms);
	}
	
	virtual shared_ptr<IScope> createScope() override {
		return bind<IScope, Scope>();
	}

	shared_ptr<IProgramContext> createProgramContext() override {
		return bind<IProgramContext, ProgramContext>();
	}

	shared_ptr<IMemoryManager> createMemoryManager() override {
		return bind<IMemoryManager, MemoryManager>();
	}

	shared_ptr<IReader> createReader(/*IDIBuilder& idiBuilder*/) {
		return bind<IReader, Reader, IDIBuilder&>(*this);
	}

	virtual shared_ptr<IProgram> createProgram(IRunContext& ctx/*IDIBuilder& idiBuilder*/) override {
		return bind<IProgram, Program, DIBuilder&, IRunContext&>(*this, ctx);
	}

	shared_ptr<IEvaluator> createEvaluator(ILispEngine* lispEngine = nullptr) {
		return bind<IEvaluator, Evaluator, DIBuilder&>(*this, lispEngine);
	}

	virtual shared_ptr<IRunContext> createRunContext(IEvaluator& evaluator) override {
		return bind<IRunContext, RunContext, IEvaluator&, DIBuilder*>(evaluator, this);
	}

	virtual shared_ptr<Printer> createPrinter() {
		return create<Printer>();
	}

	virtual shared_ptr<Repl> createRepl(ILispEngine& lispEngine) {
		return create<Repl>(lispEngine);
	}

	virtual shared_ptr<ICallResult> createCallResult() override {
		return bind<ICallResult, CallResult>();
	}

	virtual shared_ptr<ArgsList> createArgsList() override {
		return create<ArgsList>();
	}

	template<class ConcreteLispFunction>
	shared_ptr<ConcreteLispFunction> createLispFunction() {
		return create<ConcreteLispFunction>();
	}

	virtual shared_ptr<vector<PSexpr>> createPSexprCol() override {
		return create<vector<PSexpr>>();
	}

	virtual shared_ptr<NsMap> createNsMap() override {
		return create<NsMap>();
	}
	
};