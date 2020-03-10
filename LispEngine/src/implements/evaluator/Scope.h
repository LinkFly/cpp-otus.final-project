#pragma once

#include <map>

#include "../../share.h"
#include "../../interfaces/evaluator/IScope.h"
//#include "../DiBuilder.h"

using std::map;

class Scope : public IScope, public CClass {
	//friend class Scope;
	using NsMap = map<gstring, PSexpr>;
	using PNsMap = shared_ptr<NsMap>;
	PNsMap nsTable = make_shared<NsMap>();
	shared_ptr<IScope> nextScope;
	shared_ptr<IScope> parentScope;
	//DIBuilder diBuilder;
public:
	virtual void add(const gstring& name, PSexpr& sexpr) override {
		(*nsTable)[name] = sexpr;
	}
	virtual PSexpr& get(const gstring& name) override {
		return (*nsTable)[name];
	}
	virtual shared_ptr<IScope> pushNewScope() override {
		shared_ptr<Scope> newScope = make_shared<Scope>();
		newScope->parentScope = shared_ptr<IScope>(dynamic_cast<IScope*>(this));
		nextScope = std::dynamic_pointer_cast<IScope>(newScope);
		return nextScope;
	}
	virtual shared_ptr<IScope>& popScope() override {
		return parentScope;
	}
};