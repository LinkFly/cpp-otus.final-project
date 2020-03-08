#pragma once

#include <vector>

#include "../../share.h"
#include "../DataStructs.h"

using std::vector;

class IMemoryManager : public IInterface {
	/*virtual ICons createCell*/
};

class IProgramContext : public IInterface {
public:
	virtual void setScope(shared_ptr<IScope>& scope) = 0;
	virtual shared_ptr<IScope>& getScope() = 0;
};

class IProgram : public IInterface {
public:
	using FormsCollection = vector<PSexpr>;
	virtual void constructor(shared_ptr<IMemoryManager>& mman, shared_ptr<IProgramContext>& context) = 0;
	virtual FormsCollection& getProgramForms() = 0;
	virtual void addForm(PSexpr& form) = 0;
	virtual shared_ptr<IProgramContext>& getProgramContext() = 0;
};



//class LispFunction;
//class ArgsList;

class LispFunctionBase : public CClass {

};

class LispFunction : public LispFunctionBase {
public:
	virtual void call(ArgsList& args, CallResult& result) = 0;
};
