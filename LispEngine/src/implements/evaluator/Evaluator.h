#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
//#include "../../implements/DiBuilder.h"

class Evaluator : public IEvaluator, CClass {
	unique_ptr<CallResult> pLastResult;
	virtual void evalForm(PSexpr& form, CallResult& callRes) override {
		auto curCons = std::static_pointer_cast<Cons>(form);
		auto curSym = std::static_pointer_cast<Symbol>(curCons->car());
		auto nextCons = std::static_pointer_cast<Cons>(curCons->cdr());
		auto firstArg = std::static_pointer_cast<Number>(nextCons->car());
		auto nextCons2 = std::static_pointer_cast<Cons>(nextCons->cdr());
		auto secondArg = std::static_pointer_cast<Number>(nextCons2->car());
		shared_ptr<Function> func = std::static_pointer_cast<Function>(curSym->getFunction());
		ArgsList args2{ firstArg, secondArg };
		func->call(args2, callRes);
	}
public:
	virtual void eval(IProgram& program) override {
		for (PSexpr& form : program.getProgramForms()) {
			pLastResult.swap(make_unique<CallResult>());
			evalForm(form, *pLastResult.get());
		}
	}
	
	virtual CallResult& getLastResult() override {
		return *pLastResult.get();
	}
};