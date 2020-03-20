#pragma once

#include <vector>

#include "../../share.h"
#include "../i-data-structs.h"
#include "../../errors.h"
#include "../evaluator/ICallResult.h"

using std::vector;

class IMemoryManager : public IInterface {
public:
	virtual Cell* createCell() = 0;
	virtual bool freeCell(Cell* pCell) = 0;
	virtual void takeSharedPtrFunc(ILispFunction* func, shared_ptr<ILispFunction>& pFunc) = 0;
	virtual void freeSharedPtrFunc(ILispFunction* func) = 0;
};

class IPackage : public IInterface {
public:
	virtual void createSymbol(const gstring& name) = 0;
	virtual PSexpr getSymbol(const gstring& name) = 0;
	/*virtual shared_ptr<IScope> pushNewScope(shared_ptr<IScope>& parentScope) = 0;
	virtual shared_ptr<IScope>& popScope() = 0;*/
};

class IProgramContext : public IInterface {
public:
	virtual void setScope(shared_ptr<IScope>& scope) = 0;
	virtual shared_ptr<IScope>& getScope() = 0;
	/*virtual void setPackage(shared_ptr<IPackage>& scope) = 0;
	virtual shared_ptr<IPackage>& getPackage() = 0;*/
	virtual void setFnScope(shared_ptr<IScope>& scope) = 0;
	virtual shared_ptr<IScope>& getFnScope() = 0;
};

class IProgram : public IInterface {
public:
	using FormsCollection = vector<PSexpr>;

	virtual void constructor(shared_ptr<IMemoryManager>& mman, shared_ptr<IProgramContext>& context) = 0;
	virtual FormsCollection& getProgramForms() = 0;
	virtual void addForm(PSexpr& form) = 0;
	virtual shared_ptr<IProgramContext>& getProgramContext() = 0;
	virtual PSexpr createSymbol(const gstring& symName) = 0;
	virtual IRunContext& getRunContext() = 0;
	virtual PSexpr getSymByName(gstring symName) = 0;
	virtual shared_ptr<IProgram> getParentProgram() = 0;
	virtual shared_ptr<IPackage> getPackage() = 0;
};



//class LispFunction;
//class ArgsList;

class LispFunctionBase : public IInterface {

};

class ILispFunction : public LispFunctionBase {

public:
	//virtual LispFunction(IProgram)
	virtual void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) = 0;
	/*virtual void callConcrete(IRunContext& ctx, ArgsList& args, ICallResult& result) {}*/
	virtual ~ILispFunction() {

	}
};
