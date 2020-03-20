#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <set>
#include <map>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"
#include "../../implements/data-structs.h"
#include "../../interfaces/IDIBuilder.h"
#include "../../errors.h"

using std::vector;
using std::set;
using std::map;

class MemoryManager : public IMemoryManager, public CClass {
	set<Cell*> cells;
	map<ILispFunction*, shared_ptr<ILispFunction>> pfnMap;
public:
	virtual Cell* createCell() override {
		Cell* pCell{ new Cell{} };
		cells.insert(pCell);
		return pCell;
	};

	virtual bool freeCell(Cell* pCell) override {
		auto p = cells.find(pCell);
		if (p != cells.end()) {
			delete (*p);
			return true;
		}
		return false;
	}

	virtual void takeSharedPtrFunc(ILispFunction* func, shared_ptr<ILispFunction>& pFunc) override {
		pfnMap[func] = pFunc;
	}
	virtual void freeSharedPtrFunc(ILispFunction* func) override {
		pfnMap.erase(func);
	}
};

class LispFunction : public ILispFunction, public CClass {
protected:
	// TODO!!! Use ICallRes!!! (for error reaction)
	/*static */PSexpr evalArg(IRunContext& ctx, PSexpr& arg, shared_ptr<ICallResult>& res) {
		
		/*auto diBuilder = ctx.getDIBuilder();
		shared_ptr<ICallResult> pLocCallRes = diBuilder->createCallResult();*/

			//std::static_pointer_cast<ICallResult>(make_shared<CallResult>());
		//CallResult locCallRes{};

		//ctx.evalForm(arg, *pLocCallRes);

		ctx.evalForm(arg, res);
		return res->getResult();
	}
	/*virtual void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		callConcrete(ctx, args, (*res));
	}*/
};

class LispSetfFunction : public LispFunction {
	//virtual void call(ArgsList& args, Sexpr& modifiable, CallResult& result) = 0;
};

class SetfSymbolFunction : public LispSetfFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& result) override {
		// TODO Checking types
		shared_ptr<Symbol> symbol = std::static_pointer_cast<Symbol>(args.get(0));
		shared_ptr<Function> function = std::static_pointer_cast<Function>(args.get(1));
		symbol->setFunction(function);
	}
};

class Package : public IPackage, public CClass {
	map<gstring, PSexpr> nsTable;
	IDIBuilder& diBuilder;
public:
	Package(IDIBuilder& diBuilder) : diBuilder{ diBuilder } {
	
	}
	virtual void createSymbol(const gstring& name) override {
		/*nsTable[name] = diBuilder.createSymbol(name);*/
		nsTable[name] = shared_ptr<Symbol>(new Symbol(name));
	}
	virtual PSexpr getSymbol(const gstring& name) override {
		PSexpr sym = nsTable[name];
		if (sym.get() != nullptr) {
			return sym;
		}
		createSymbol(name);
		return nsTable[name];
	}
	/*virtual shared_ptr<IScope> pushNewScope(shared_ptr<IScope>& parentScope) = 0;
	virtual shared_ptr<IScope>& popScope() = 0;*/
};

class ProgramContext : public IProgramContext, public CClass {
	shared_ptr<IScope> scope;
	shared_ptr<IScope> fnScope;
public:
	virtual void setScope(shared_ptr<IScope>& scope) override {
		this->scope = scope;
	}
	virtual shared_ptr<IScope>& getScope() override {
		return scope;
	}
	virtual void setFnScope(shared_ptr<IScope>& curFnScope) override {
		this->fnScope = curFnScope;
	}
	virtual shared_ptr<IScope>& getFnScope() override {
		return fnScope;
	}
};

class Program : public IProgram, public CClass {
public:
	using FormsCollection = vector<PSexpr>;
private:
	FormsCollection forms;
	shared_ptr<IMemoryManager> mman;
	shared_ptr<IProgramContext> context;
	IDIBuilder& diBuilder;
	IRunContext& ctx;
public:
	Program(IDIBuilder& idiBuilder, IRunContext& ctx) : diBuilder{ idiBuilder }, ctx { ctx } {
		shared_ptr<IMemoryManager> mman = diBuilder.createMemoryManager();
		shared_ptr<IProgramContext> context = diBuilder.createProgramContext();
		constructor(mman, context);
	}
	virtual void constructor(shared_ptr<IMemoryManager>& mman, shared_ptr<IProgramContext>& context) override {
		this->mman = mman;
		this->context = context;
	}
	virtual FormsCollection& getProgramForms() override {
		return forms;
	}
	virtual void addForm(PSexpr& form) override {
		forms.push_back(form);
	}
	virtual shared_ptr<IProgramContext>& getProgramContext() override {
		return context;
	}

	virtual PSexpr createSymbol(const gstring& symName) override {
		PSexpr symSexpr = diBuilder.createSymbol(symName);
		getProgramContext()->getScope()->add(symName, symSexpr);
		return symSexpr;
	}

	virtual IRunContext& getRunContext() override {
		return ctx;
	}

	//virtual void setRunContext(IRunContext& ctx) override {
	//	this->ctx = ctx;
	//}

	virtual PSexpr getSymByName(gstring symName) override {
		shared_ptr<IPackage>& package = getPackage();
		/*return getProgramContext()->getScope()->get(symName);*/
		return package->getSymbol(symName);
	}

	virtual shared_ptr<IProgram> getParentProgram() override {
		auto& parentCtx = getRunContext().getParentContext();
		shared_ptr<IProgram> res;
		if (parentCtx.get() != nullptr) {
			res = parentCtx->getProgram();
		}
		return res;
	}

	virtual shared_ptr<IPackage> getPackage() override {
		/*std::static_pointer_cast<IPackage>(*/
		/*static shared_ptr<IPackage> package = make_shared<Package>(diBuilder);*/
		static shared_ptr<IPackage> package = diBuilder.createPackage();
		return package;
	}
};

//class PlusLispFunction : public LispFunction {
//public:
//	//void call(ArgsList& args, CallResult& res) override {
//	//	auto oneArg = reinterpret_cast<Number*>(args.get(0).get());
//	//	auto twoArg = reinterpret_cast<Number*>(args.get(1).get());
//	//	if (oneArg == nullptr || twoArg == nullptr) {
//	//		cerr << "Bad args\n";
//	//		exit(-1);
//	//	}
//	//	//Sexpr* resNumber = new Number(oneArg->getValue() + twoArg->getValue());
//	//	shared_ptr<Sexpr> resNumber = std::static_pointer_cast<Sexpr>(
//	//		make_shared<Number>(oneArg->getValue() + twoArg->getValue()));
//	//	res.setResult(resNumber, nullptr /*[resNumber]() {
//	//		delete resNumber;
//	//		}*/);
//	//}
//};
