#pragma once

#include "../share.h"

class ILispEngine: public IInterface {
public:
	virtual gstring evalSexprStr(gstring& sexprStr) = 0;
	virtual PSexpr getLastPSexprRes() = 0;
	virtual Printer getPrinter() = 0;
	virtual shared_ptr<ICallResult> getLastCallResult() = 0;
	virtual void setQuit() = 0;
	virtual bool getQuit() = 0;
	virtual gstring evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult> callRes) = 0;
};