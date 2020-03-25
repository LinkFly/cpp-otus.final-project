#pragma once

#include <memory>

#include "../base/share.h"
#include "../interfaces/core/evaluator/IScope.h"
#include "../interfaces/core/evaluator/IEvaluator.h"
//#include "../implements/data-structs.h"

using std::shared_ptr;


// TODO Rename (not std interface (contained template-functions)
class IDIBuilder : public IInterface {
public:
	template<class T, typename ...Args>
	shared_ptr<T> create(Args&... args) {
		return make_shared<T>(args...);
	}

	template<class TInterface, class TImplement = TInterface, class ...Args>
	shared_ptr<TInterface> bind(Args... args) {
		return std::static_pointer_cast<TInterface>(make_shared<TImplement>(args...));
	}

	template<class T, typename ...Args>
	shared_ptr<T> createError(Args&... args) {
		return make_shared<T>(args...);
	}

	virtual shared_ptr<IScope> createScope() = 0;
	virtual shared_ptr<Number> createNumber(int64_t num) = 0;
	virtual shared_ptr<Nil> createNil() = 0;
	virtual shared_ptr<IPackage> createPackage(/*const gstring& symName*/) = 0;
	virtual shared_ptr<Lambda> createLambda(PSexpr params, PSexpr forms) = 0;
	virtual shared_ptr<Symbol> createSymbol(const gstring& symName) = 0;
	virtual shared_ptr<String> createString(const gstring& symName) = 0;

	template<class SexprChild1, class SexprChild2>
	shared_ptr<Cons> createCons(SexprChild1& sexpr1OrChild, SexprChild2& sexpr2OrChild) {
		auto sexpr1 = std::static_pointer_cast<Sexpr>(sexpr1OrChild);
		auto sexpr2 = std::static_pointer_cast<Sexpr>(sexpr2OrChild);
		return shared_ptr<Cons>(new Cons(sexpr1, sexpr2));
	}

	virtual shared_ptr<IProgramContext> createProgramContext() = 0;
	virtual shared_ptr<IProgram> createProgram(IRunContext& ctx/*IDIBuilder& idiBuilder*/) = 0;
	virtual shared_ptr<IMemoryManager> createMemoryManager() = 0;
	virtual shared_ptr<IRunContext> createRunContext(IEvaluator& evaluator) = 0;
	virtual shared_ptr<ICallResult> createCallResult() = 0;
	virtual shared_ptr<ArgsList> createArgsList() = 0;
	virtual shared_ptr<vector<PSexpr>> createPSexprCol() = 0;
	virtual shared_ptr<NsMap> createNsMap() = 0;
};
