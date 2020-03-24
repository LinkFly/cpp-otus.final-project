#pragma once

#include <map>

#include "../../../base/share.h"
#include "../../../interfaces/core/evaluator/IScope.h"
//#include "../../DiBuilder.h"

using std::map;

class Scope : public IScope, public CClass {
	PNsMap nsTable = getGlobal().getIDIBuilder()->createNsMap();
	shared_ptr<IScope> parentScope;
public:
	Scope() {

	}
	virtual void add(const gstring& name, PSexpr& sexpr) override {
		(*nsTable)[name] = sexpr;
	}
	virtual PSexpr& get(const gstring& name) override {
		return (*nsTable)[name];
	}
	virtual PSexpr find(const gstring& name) override {
		auto it = nsTable->find(name);
		if (it != nsTable->end()) {
			return it->second;
		}
		else {
			PSexpr res;
			if (parentScope.get() != nullptr) {
				res = parentScope->find(name);
			}
			return res;
		}
	}
	virtual void setParentScope(shared_ptr<IScope> parentScope) override {
		this->parentScope = parentScope;
	}
	virtual shared_ptr<IScope> pushNewScope(shared_ptr<IScope> parentScope) override {
		shared_ptr<IScope> newScope = getGlobal().getIDIBuilder()->createScope();
		newScope->setParentScope(parentScope);
		return newScope;
	}
	virtual shared_ptr<IScope> popScope() override {
		return parentScope;
	}
};