#pragma once

#include "../../share.h"

class IScope : public IInterface {
public:
	virtual void add(gstring& name, PSexpr& sexpr) = 0;
	virtual PSexpr& get(gstring& name) = 0;
	virtual shared_ptr<IScope> pushNewScope() = 0;
	virtual shared_ptr<IScope>& popScope() = 0;
};