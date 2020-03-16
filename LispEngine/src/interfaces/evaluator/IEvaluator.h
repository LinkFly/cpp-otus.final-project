#pragma once

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"

class IEvaluator : public IInterface {
public:
	virtual void evalForm(PSexpr& form, ICallResult& callRes) = 0;
	virtual void eval(IProgram& program, ErrorCallback onErrorCallback) = 0;
	virtual ICallResult& getLastResult() = 0;
	/*virtual void setRepl(Repl& repl) = 0;
	virtual Repl& getRepl() = 0;*/
	virtual void createRunContext(shared_ptr<IScope>& scope, shared_ptr<IScope>& fnScope, bool isNewDebugLevel) = 0;
};