#pragma once

#include <vector>

#include "../../share.h"
#include "../DataStructs.h"

using std::vector;

class IMemoryManager : public IInterface {
public:
	virtual Cell* createCell() = 0;
	virtual bool freeCell(Cell* pCell) = 0;
	virtual void takeSharedPtrFunc(ILispFunction* func, shared_ptr<ILispFunction>& pFunc) = 0;
	virtual void freeSharedPtrFunc(ILispFunction* func) = 0;
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
	virtual PSexpr& createSymbol(const gstring& symName) = 0;
};



//class LispFunction;
//class ArgsList;

class LispFunctionBase : public IInterface {

};

class IRunContext: public IInterface {
public:
	virtual void evalForm(PSexpr& sexpr, CallResult& callRes) = 0;
};

class ILispFunction : public LispFunctionBase {

public:
	//virtual LispFunction(IProgram)
	virtual void call(IRunContext& ctx, ArgsList& args, CallResult& result) = 0;
	virtual ~ILispFunction() {

	}
};
