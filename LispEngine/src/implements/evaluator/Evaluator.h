#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
#include "../../interfaces/IDiBuilder.h"
//#include "../../implements/DiBuilder.h"
#include "../../interfaces/ILispEngine.h"

class Evaluator : public IEvaluator, public CClass {
	shared_ptr<ICallResult> pLastResult;
	IDIBuilder& diBuilder;
	ILispEngine* lispEngine;
public:
	Evaluator(IDIBuilder& diBuilder, ILispEngine* lispEngine) : 
		diBuilder{ diBuilder }, lispEngine{ lispEngine } {}

	/*virtual void evalForm(PSexpr& form, shared_ptr<ICallResult> callRes) override {
		evalForm(form, *callRes);
	}*/
	virtual void evalForm(PSexpr& form, shared_ptr<ICallResult> callRes) override {
		if (!form->isCons()) {
			if (form->isSymbol()) {
				auto sym = std::static_pointer_cast<Symbol>(form);
				if (sym->getSelfEval()) {
					callRes->setResult(sym, nullptr);
				}
				else {
					auto boundedValue = sym->getValue();
					if (boundedValue.get() == nullptr) {
						auto symName = sym->getName();
						callRes->setErrorResult(make_shared<ErrorSymbolUnbound>(symName));
						return;
					}
					callRes->setResult(sym->getValue(), nullptr);
				}
				return;
			}
			callRes->setResult(form, nullptr);
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

	virtual void evalSexprStr(gstring& sexprStr, ICallResult& callRes) override {
		lispEngine->evalSexprStr(sexprStr);
		callRes = *lispEngine->getLastCallResult();
	}

	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult>& callRes) override {
		//callRes = lispEngine->getLastCallResult();
		lispEngine->evalSexprStr(sexprStr, callRes);
		//callRes = lispEngine->getLastCallResult();
	}

	virtual void setQuit() override {
		lispEngine->setQuit();
	}
	virtual void createRunContext(shared_ptr<IScope>& scope, shared_ptr<IScope>& fnScope, bool isNewDebugLevel) override {
		//shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*this);
		//shared_ptr<IRunContext> actualCtx;
		auto& actualCtx = getGlobal().getRunContext();
		bool bCurCtxIsTopLevel = actualCtx->isTopLevel();
		/*if (curCtx.get() != nullptr) {
			actualCtx = curCtx;
			bCurCtxIsTopLevel = false;
		}
		else {
			actualCtx = getGlobal().getTopLevelRunContext();
			bCurCtxIsTopLevel = true;
		}*/
		shared_ptr<IRunContext> ctx;
		if (!(isNewDebugLevel || bCurCtxIsTopLevel)) {
			actualCtx = actualCtx->popContext();
		}
		ctx = actualCtx->pushNewContext(actualCtx);
		ctx->getProgram()->getProgramContext()->setScope(scope);
		ctx->getProgram()->getProgramContext()->setFnScope(fnScope);
		getGlobal().setRunContext(ctx);
	}

	shared_ptr<IRunContext> getRunContext() {
		return getGlobal().getRunContext();
	}

	virtual void eval(IProgram& program, shared_ptr<ICallResult> callRes, ErrorCallback onErrorCallback = nullptr) override {
		/*createRunContext();*/
		getRunContext()->setOnErrorCallback(onErrorCallback);
		getRunContext()->setDIBuilder(&diBuilder);
		for (PSexpr& form : program.getProgramForms()) {
			/*shared_ptr<ICallResult> pCallRes = 
				std::static_pointer_cast<ICallResult>(make_shared<CallResult>());*/
			shared_ptr<ICallResult> pCallRes;
			if (callRes.get() == nullptr) {
				pCallRes = diBuilder.createCallResult();
			}
			else {
				pCallRes = callRes;
			}
			//shared_ptr<ICallResult> pCallRes;
			pLastResult.swap(pCallRes);
			evalForm(form, pLastResult);
			if (pLastResult->getStatus() == EResultStatus::error) {
				auto& global = getGlobal();
				ErrorCallback& errCallback = global.getRunContext()->getOnErrorCallback();
				errCallback(pLastResult->getLastError());
			}
		}
	}

	virtual ICallResult& getLastResult() override {
		return *pLastResult.get();
	}

	virtual shared_ptr<ICallResult> getPLastResult() override {
		return pLastResult;
	}
};