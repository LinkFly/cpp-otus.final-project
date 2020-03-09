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
		auto firstArgSexpr = nextCons->car();
		
		if (firstArgSexpr->isCons()) {
			CallResult locCallRes;
			evalForm(firstArgSexpr, locCallRes);
			firstArgSexpr = locCallRes.result;
		}
		auto firstArg = std::static_pointer_cast<Number>(firstArgSexpr);

		auto nextCons2 = std::static_pointer_cast<Cons>(nextCons->cdr());
		auto secondArgSexpr = nextCons2->car();
		if (secondArgSexpr->isCons()) {
			CallResult locCallRes;
			evalForm(secondArgSexpr, locCallRes);
			secondArgSexpr = locCallRes.result;
		}
		auto secondArg = std::static_pointer_cast<Number>(secondArgSexpr);

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