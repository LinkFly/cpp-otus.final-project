#pragma once

#include "../../share.h"

//

#include "../../interfaces/evaluator/IRunContext.h"

class RunContext : public IRunContext, public CClass {
	IEvaluator& evaluator;
	shared_ptr<IRunContext> createRunContext() {
		shared_ptr<IRunContext> ctx = 
			std::static_pointer_cast<IRunContext>(make_shared<RunContext>(evaluator));
		ctx->getParentContext().reset(this);
		// TODO setLevel/getLevel rename to setDebugLevel/getDebugLevel
		ctx->setLevel(getLevel() + 1);
		return ctx;
	}
	uint8_t level;
	shared_ptr<Error> error = Error::getNoError();

	ErrorCallback onErrorCallback;
	shared_ptr<IRunContext> parentCtx;
	
	IDIBuilder* diBuilder;
	EResultStatus status;
public:
	
	RunContext(IEvaluator& evaluator) : evaluator{ evaluator }, level{ 0 } {}

	virtual void evalForm(PSexpr& sexpr, ICallResult& callRes) override {
		evaluator.evalForm(sexpr, callRes);
	}

	virtual uint8_t getLevel() override {
		return level;
	}

	virtual void setLevel(uint8_t level) override {
		this->level = level;
	}

	virtual shared_ptr<IRunContext> pushNewContext() override {
		return createRunContext();
	}

	virtual shared_ptr<IRunContext> popContext() override {
		auto res = parentCtx;
		return res;
	}

	virtual void setOnErrorCallback(ErrorCallback callback) override {
		onErrorCallback = callback;
	}

	virtual ErrorCallback& getOnErrorCallback() override {
		return onErrorCallback;
	}
	/////////////////////////////////////
	virtual EResultStatus getStatus() override {
		return status;
	}
	virtual void setStatus(EResultStatus status) override {
		this->status = status;
	}
	virtual void setStatus(EResultStatus status, std::function<void()> deleter) override {

	}

	//virtual PSexpr getResult() override {

	//}
	//virtual void setResult(PSexpr result) override {

	//}
	virtual shared_ptr<Error>& getLastError() override {
		return error;
	}
	virtual void setLastError(shared_ptr<Error>& error) override {
		this->error = error;
	}


	////////////////////////////////////////////

	virtual shared_ptr<IRunContext>& getParentContext() override {
		return std::static_pointer_cast<IRunContext>(parentCtx);
	}
	virtual void setParentContext(shared_ptr<IRunContext>& runCtx) override {
		parentCtx = runCtx;
	}

	//virtual shared_ptr<IRunContext>& getParentContext() override {
	//	return std::static_pointer_cast<IRunContext>(parentCtx);
	//}

	//virtual void setParentContext(shared_ptr<IRunContext>& runCtx) override {
	//	parentCtx = runCtx;
	//}

	//////////////////////////
	virtual IDIBuilder* getDIBuilder() override {
		return diBuilder;
	}

	virtual void setDIBuilder(IDIBuilder* diBuilder) override {
		this->diBuilder = diBuilder;
	}

};