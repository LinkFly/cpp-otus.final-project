#pragma once

#include <memory>

#include "../share.h"
#include "../interfaces/evaluator/IScope.h"
#include "../interfaces/evaluator/IEvaluator.h"
#include "../implements/data-structs.h"

using std::shared_ptr;

class IDIBuilder : public IInterface {
public:
	//template<class Interface>
	///*virtual T& get() = 0;*/ //denied virtual
	//shared_ptr<Interface>& get() {};

	//template<class Interface, typename ...Args>
	///*virtual T create(Args... args) = 0;*/ //denied virtual
	//shared_ptr<Interface> create(Args... args) { return shared_ptr<Interface>{nullptr}; }

	virtual shared_ptr<IScope> createScope() = 0;
	virtual shared_ptr<Number> createNumber(int32_t num) = 0;
	virtual shared_ptr<Nil> createNil() = 0;
	virtual shared_ptr<Symbol> createSymbol(const gstring& symName) = 0;

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

};
