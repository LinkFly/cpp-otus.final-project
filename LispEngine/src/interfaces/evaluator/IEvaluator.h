#pragma once

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"

class IEvaluator : public IInterface {
protected:
	virtual void evalForm(PSexpr& form, CallResult& callRes) = 0;
public:
	virtual void eval(IProgram& program) = 0;
	virtual CallResult& getLastResult() = 0;
};