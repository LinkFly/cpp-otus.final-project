#pragma once

#include "../../base/share.h"
#include "evaluator/IProgram.h"

class IReader : public IInterface {
public:
	virtual void read(gstring& programText, IProgram& program) = 0;
	virtual PSexpr parseSymbol(gstring token, IProgram& program) = 0;
	virtual PSexpr parseString(gstring token, IProgram& program) = 0;
};