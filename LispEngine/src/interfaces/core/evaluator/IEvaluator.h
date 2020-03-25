#pragma once

#include "../../../base/share.h"
#include "../../../interfaces/core/evaluator/i-program-structs.h"

class IEvaluator : public IInterface {
public:
	virtual void createRunContext(shared_ptr<IScope>& scope, shared_ptr<IScope>& fnScope, bool isNewDebugLevel) = 0;

	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult>& callRes) = 0;
	virtual void evalProgram(shared_ptr<IRunContext> ctx, shared_ptr<ICallResult> callRes = nullptr) = 0;
	virtual void eval(shared_ptr<IRunContext> ctx, shared_ptr<ICallResult> callRes, ErrorCallback onErrorCallback = nullptr) = 0;
	virtual void evalForm(PSexpr& form, shared_ptr<ICallResult> callRes) = 0;

	virtual ICallResult& getLastResult() = 0;
	virtual shared_ptr<ICallResult> getPLastResult() = 0;

	virtual void setQuit() = 0;
};