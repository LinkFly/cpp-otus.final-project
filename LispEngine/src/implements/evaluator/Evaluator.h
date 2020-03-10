#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
//#include "../../implements/DiBuilder.h"

class Evaluator : public IEvaluator, CClass {
	unique_ptr<CallResult> pLastResult;
	virtual void evalForm(PSexpr& form, CallResult& callRes) override {
		if (!form->isCons()) {
			if (form->isSymbol()) {
				auto sym = std::static_pointer_cast<Symbol>(form);
				callRes.setResult(sym->getValue(), nullptr);
				return;
			}
			callRes.setResult(form, nullptr);
			return;
		}
		auto curCons = std::static_pointer_cast<Cons>(form);
		auto curSym = std::static_pointer_cast<Symbol>(curCons->car());
		auto nextSexpr = curCons->cdr();
		ArgsList args{};
		while (nextSexpr->isCons()) {
			auto nextCons = std::static_pointer_cast<Cons>(nextSexpr);
			auto nextArg = nextCons->car();
			if (nextArg->isCons()) {
				CallResult locCallRes;
				evalForm(nextArg, locCallRes);
				nextArg = locCallRes.result;
			}
			args.addArg(nextArg);
			nextSexpr = nextCons->cdr();
		}
		shared_ptr<Function> func = std::static_pointer_cast<Function>(curSym->getFunction());
		func->call(args, callRes);
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