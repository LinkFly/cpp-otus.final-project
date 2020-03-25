#pragma once

#include <memory>

#include "../../../base/share.h"
#include "../../../interfaces/core/evaluator/IProgram.h"
#include "../../../interfaces/IDiBuilder.h"
#include "../../../interfaces/core/ILispEngine.h"
#include "../../../interfaces/core/IRepl.h"


class Evaluator : public IEvaluator, public CClass {
	shared_ptr<ICallResult> pLastResult;
	IDIBuilder& diBuilder;
	ILispEngine* lispEngine;

	void errHandler() {
		auto& global = getGlobal();
		ErrorCallback& errCallback = global.getRunContext()->getOnErrorCallback();
		errCallback(pLastResult->getLastError());
	}
public:
	Evaluator(IDIBuilder& diBuilder, ILispEngine* lispEngine) : 
		diBuilder{ diBuilder }, lispEngine{ lispEngine } {}

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
						auto& diBuilder = getGlobal().getIDIBuilder();
						callRes->setErrorResult(diBuilder->createError<ErrorSymbolUnbound>(symName));
						errHandler();
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
			args.addArg(nextArg);
			nextSexpr = nextCons->cdr();
		}
		shared_ptr<Function> func = std::static_pointer_cast<Function>(curSym->getFunction());
		func->call(args, callRes);
		//////////////
		if (callRes->getStatus() == EResultStatus::error) {
			errHandler();
		}
	}

	//virtual void evalSexprStr(gstring& sexprStr, ICallResult& callRes) override {
	//	lispEngine->evalSexprStr(sexprStr);
	//	callRes = *lispEngine->getLastCallResult();
	//}

	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult>& callRes) override {
		//callRes = lispEngine->getLastCallResult();
		lispEngine->evalSexprStr(sexprStr, callRes);
		//callRes = lispEngine->getLastCallResult();
	}

	virtual void setQuit() override {
		lispEngine->setQuit();
	}
	virtual void createRunContext(shared_ptr<IScope>& scope, shared_ptr<IScope>& fnScope, 
		bool isNewDebugLevel) override
	{
		auto& actualCtx = getRunContext();
		bool bCurCtxIsTopLevel = actualCtx->isTopLevel();

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

	virtual void eval(shared_ptr<IRunContext> ctx, shared_ptr<ICallResult> callRes, ErrorCallback onErrorCallback = nullptr) override {
		auto& program = ctx->getProgram();
		ctx->setOnErrorCallback(onErrorCallback);
		ctx->setDIBuilder(&diBuilder);
		for (PSexpr& form : program->getProgramForms()) {
			shared_ptr<ICallResult> pCallRes;
			if (callRes.get() == nullptr) {
				pCallRes = diBuilder.createCallResult();
			}
			else {
				pCallRes = callRes;
			}

			pLastResult.swap(pCallRes);
			evalForm(form, pLastResult);
		}
	}

	virtual void evalProgram(shared_ptr<IRunContext> ctx, shared_ptr<ICallResult> callRes = nullptr) override {
		eval(getRunContext(), callRes, [this](shared_ptr<Error>& err) {
			defaultErrHandler(err);
			});
	}

	void defaultErrHandler(shared_ptr<Error>& err) {
		auto& lastCtx = getGlobal().getRunContext();
		auto& progCtx = lastCtx->getProgram()->getProgramContext();
		auto& fnScope = progCtx->getFnScope();
		auto& scope = progCtx->getScope();
		shared_ptr<IRepl>& repl = lispEngine->getRepl();

		createRunContext(scope, fnScope, true);

		repl->printError(err);
		auto& ctx = getGlobal().getRunContext();
		ctx->setLastError(err);

		repl->run(ctx);
	}

	virtual ICallResult& getLastResult() override {
		return *pLastResult.get();
	}

	virtual shared_ptr<ICallResult> getPLastResult() override {
		return pLastResult;
	}
};