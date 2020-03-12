#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
#include "../../interfaces/IDiBuilder.h"
//#include "../../implements/DiBuilder.h"

class Evaluator : public IEvaluator, public CClass {
	shared_ptr<ICallResult> pLastResult;
	IDIBuilder& diBuilder;
public:
	Evaluator(IDIBuilder& diBuilder) : diBuilder{ diBuilder } {}
	virtual void evalForm(PSexpr& form, ICallResult& callRes) override {
		if (!form->isCons()) {
			if (form->isSymbol()) {
				auto sym = std::static_pointer_cast<Symbol>(form);
				if (sym->getSelfEval()) {
					callRes.setResult(sym, nullptr);
				}
				else {
					callRes.setResult(sym->getValue(), nullptr);
				}
				
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
	shared_ptr<IRunContext>& getRunContext() {
		return getGlobal().getRunContext();
	}

	virtual void eval(IProgram& program, ErrorCallback onErrorCallback = nullptr) override {
		createRunContext();
		getRunContext()->setOnErrorCallback(onErrorCallback);
		getRunContext()->setDIBuilder(&diBuilder);
		for (PSexpr& form : program.getProgramForms()) {
			/*shared_ptr<ICallResult> pCallRes = 
				std::static_pointer_cast<ICallResult>(make_shared<CallResult>());*/
			auto pCallRes = diBuilder.createCallResult();
			//shared_ptr<ICallResult> pCallRes;
			pLastResult.swap(pCallRes);
			evalForm(form, *pLastResult.get());
		}
	}

	virtual ICallResult& getLastResult() override {
		return *pLastResult.get();
	}
};