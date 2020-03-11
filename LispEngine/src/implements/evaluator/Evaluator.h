#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
#include "../../interfaces/IDiBuilder.h"
//#include "../../implements/DiBuilder.h"

class Evaluator : public IEvaluator, public CClass {
	unique_ptr<CallResult> pLastResult;
	IDIBuilder& diBuilder;
public:
	Evaluator(IDIBuilder& diBuilder) : diBuilder{ diBuilder } {}
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
			/*if (nextArg->isCons()) {
				CallResult locCallRes;
				evalForm(nextArg, locCallRes);
				nextArg = locCallRes.result;
			}*/
			args.addArg(nextArg);
			nextSexpr = nextCons->cdr();
		}
		shared_ptr<Function> func = std::static_pointer_cast<Function>(curSym->getFunction());
		func->call(args, callRes);
	}

	void createRunContext() {
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*this);
		getGlobal().setRunContext(ctx);
	}

	virtual void eval(IProgram& program) override {
		createRunContext();
		for (PSexpr& form : program.getProgramForms()) {
			pLastResult.swap(make_unique<CallResult>());
			evalForm(form, *pLastResult.get());
		}
	}

	virtual CallResult& getLastResult() override {
		return *pLastResult.get();
	}
};