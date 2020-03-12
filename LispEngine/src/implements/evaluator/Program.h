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

class ArgsList : public CClass {
	vector<PSexpr> args;
public:
	ArgsList() {
		args.resize(0);
	}
	ArgsList(shared_ptr<Sexpr> one) {
		args.resize(1);
		args[0] = one;
	}
	ArgsList(shared_ptr<Sexpr> one, shared_ptr<Sexpr> two) {
		args.resize(2);
		args[0] = one;
		args[1] = two;
	}
	void addArg(PSexpr& sexpr) {
		args.push_back(sexpr);
	}
	shared_ptr<Sexpr> get(short_size argPos) {
		return args[argPos];
	}
	size_t size() {
		return args.size();
	}
};

class LispFunction : public ILispFunction, public CClass {
protected:
	PSexpr evalArg(IRunContext& ctx, PSexpr& arg) {
		auto diBuilder = ctx.getDIBuilder();
		shared_ptr<ICallResult> pLocCallRes = diBuilder->createCallResult();
			//std::static_pointer_cast<ICallResult>(make_shared<CallResult>());
		//CallResult locCallRes{};
		ctx.evalForm(arg, *pLocCallRes);
		return pLocCallRes->getResult();
	}
};

class LispSetfFunction : public LispFunction {
	//virtual void call(ArgsList& args, Sexpr& modifiable, CallResult& result) = 0;
};

class SetfSymbolFunction : public LispSetfFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& result) override {
		// TODO Checking types
		shared_ptr<Symbol> symbol = std::static_pointer_cast<Symbol>(args.get(0));
		shared_ptr<Function> function = std::static_pointer_cast<Function>(args.get(1));
		symbol->setFunction(function);
	}
};

class ProgramContext : public IProgramContext, public CClass {
	shared_ptr<IScope> scope;
public:
	virtual void setScope(shared_ptr<IScope>& scope) override {
		this->scope = scope;
	}
	virtual shared_ptr<IScope>& getScope() override {
		return scope;
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
public:
	Program(IDIBuilder& idiBuilder) : diBuilder{ idiBuilder } {
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

	virtual PSexpr& createSymbol(const gstring& symName) override {
		PSexpr symSexpr = diBuilder.createSymbol(symName);
		getProgramContext()->getScope()->add(symName, symSexpr);
		return symSexpr;
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
