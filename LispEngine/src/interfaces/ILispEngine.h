#pragma once

#include "../share.h"

class ILispEngine: public IInterface {
public:
	virtual gstring evalSexprStr(gstring& sexprStr) = 0;
};