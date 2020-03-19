#pragma once

#include "../share.h"

class ILispEngine: public IInterface {
public:
	virtual gstring evalSexprStr(gstring& sexprStr) = 0;
	virtual PSexpr getLastPSexprRes() = 0;
	virtual Printer getPrinter() = 0;
};