#pragma once

#include <memory>
#include <tuple>

#include "../../base/share.h"
#include "../../interfaces/core/evaluator/i-program-structs.h"
#include "../../interfaces/core/evaluator/IRunContext.h"
#include "../../interfaces/core/evaluator/ICallResult.h"
#include "../../interfaces/IDIBuilder.h"

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

struct TypeBaseStruct {

};

struct NilStruct : TypeBaseStruct {
	void* nil = nullptr;
};

struct NumberStruct : TypeBaseStruct {
	int64_t num;
	int64_t operator=(int64_t val) {
		return num = val;
	}
	operator int64_t() {
		return num;
	}
};

class CustomType : TypeBaseStruct {

};

struct ConsStruct {
	Cell* pCell;
};

struct SymbolDesc {
	gstring name;
	shared_ptr<Sexpr> bound;
	shared_ptr<Sexpr> fbound;
	bool bIsSelfEvaluated = false;
};

class Cons;

class LambdaBase : public CClass {
public:
	virtual void call(ArgsList& args, shared_ptr<ICallResult>& callRes) = 0;
};

struct LambdaDesc {
	PSexpr params;
	PSexpr forms;
	LambdaBase* lambda;
};

struct SymbolStruct {
	SymbolDesc* symDesc;
};

struct CustomTypeStruct {
	CustomType* pCustom;
};

enum class EFunctionType {
	standard, lambda
};
struct FunctionDesc {
	EFunctionType type = EFunctionType::standard;
	ILispFunction* func = nullptr;
	LambdaDesc* lambdaDesc;
};

struct StringDesc {
	gstring str;
};

struct FunctionStruct {
	FunctionDesc* funcDesc;
};

struct StringStruct {
	StringDesc* strDesc;
};

union TypeStruct {
	NilStruct nil;
	NumberStruct number;
	StringStruct string;
	SymbolStruct symbol;
	ConsStruct cons;
	FunctionStruct function;
	CustomTypeStruct custom;
public:
	TypeStruct() {};
	~TypeStruct() {};
};

class DynamicType : public CClass {
public:
	ETypeId typeId;
	TypeStruct tstruct;
};

class Sexpr : public CBaseClass {
protected:
	DynamicType dtype;
public:
	// for debug
	DynamicType& _getDType() { return dtype; }
	bool isCons() {
		return dtype.typeId == ETypeId::cons;
	}
	bool isAtom() {
		return !isCons();
	}
	bool isSymbol() {
		return dtype.typeId == ETypeId::symbol;
	}
	bool isNil() {
		return dtype.typeId == ETypeId::nil;
	}
	bool isNumber() {
		return dtype.typeId == ETypeId::number;
	}
	bool isString() {
		return dtype.typeId == ETypeId::string;
	}
	bool isFunction() {
		return dtype.typeId == ETypeId::function || dtype.typeId == ETypeId::lambda;
	}
	bool isLambda() {
		return dtype.typeId == ETypeId::lambda;
	}
	bool isCustom() {
		return dtype.typeId == ETypeId::custom;
	}
};

class Cell {
public:
	PSexpr car;
	PSexpr cdr;
};

class Atom : public Sexpr {

};

class Cons : public Sexpr {
	IMemoryManager& getMemMan() {
		auto& global = getGlobal();
		return global.getMemoryManager();
	}
public:
	static shared_ptr<Cons> AsCons(PSexpr& psexpr) {
		return std::static_pointer_cast<Cons>(psexpr);
	}
	Cons(PSexpr& car, PSexpr& cdr) {
		dtype.typeId = ETypeId::cons;
		Cell*& pCell = dtype.tstruct.cons.pCell;
		pCell = getMemMan().createCell();
		pCell->car = car;
		pCell->cdr = cdr;
	}
	~Cons() {
		getMemMan().freeCell(dtype.tstruct.cons.pCell);
	}
	PSexpr car() {
		return dtype.tstruct.cons.pCell->car;
	}
	PSexpr cdr() {
		return dtype.tstruct.cons.pCell->cdr;
	}
	void car(PSexpr val) {
		dtype.tstruct.cons.pCell->car = val;
	}
	void cdr(PSexpr val) {
		dtype.tstruct.cons.pCell->cdr = val;
	}
};

class Number : public Atom {
public:
	static shared_ptr<Number> AsNumber(const PSexpr& psexpr) {
		return std::static_pointer_cast<Number>(psexpr);
	}
	Number() : Number(0) {}
	Number(int64_t val) {
		dtype.typeId = ETypeId::number;
		dtype.tstruct.number = val;
	}
	int64_t getValue() {
		return dtype.tstruct.number;
	}
};

class String : public Atom {
public:
	String() : String("") {}
	String(const gstring& str = "") {
		dtype.typeId = ETypeId::string;
		auto strDesc = new StringDesc{};
		strDesc->str = str;
		dtype.tstruct.string.strDesc = strDesc;
	}
	~String() {
		delete dtype.tstruct.string.strDesc;
	}
	gstring& getValue() {
		return dtype.tstruct.string.strDesc->str;
	}
};

class Nil : public Atom {
public:
	Nil() {
		dtype.typeId = ETypeId::nil;
	}
	bool isTrueNil() {
		return dtype.typeId == ETypeId::nil && getValue() == nullptr;
	}
	Cons* getValue() {
		return nullptr;
	}
};

class Lambda;

class Caller {
	std::function<void(ArgsList&, ICallResult&)>& fn;
public:
	Caller(std::function<void(ArgsList&, ICallResult&)>& fn) : fn{ fn } {}
	void operator()(ArgsList& args, ICallResult& result) {
		fn(args, result);
	}
};

class Function : public Atom {
	//shared_ptr<LispFunction> func;
	ILispFunction& getFunc() {
		return *dtype.tstruct.function.funcDesc->func;
	}
	bool isLambda() {
		return dtype.tstruct.function.funcDesc->type == EFunctionType::lambda;
	}
	IMemoryManager& getMemMan() {
		return getGlobal().getMemoryManager();
	}
protected:
	shared_ptr<IRunContext> getRunContext() {
		auto global = getGlobal();
		return global.getRunContext();
	}
public:
	Function() = default;
	Function(shared_ptr<ILispFunction>& lispFunc) {
		dtype.typeId = ETypeId::function;
		dtype.tstruct.function.funcDesc = new FunctionDesc();
		dtype.tstruct.function.funcDesc->func = lispFunc.get();
		getMemMan().takeSharedPtrFunc(lispFunc.get(), lispFunc);
	}
	~Function() {
		if (dtype.tstruct.function.funcDesc != nullptr) {
			getMemMan().freeSharedPtrFunc(dtype.tstruct.function.funcDesc->func);
			if (dtype.tstruct.function.funcDesc != nullptr) {
				delete dtype.tstruct.function.funcDesc;
				dtype.tstruct.function.funcDesc = nullptr;
			}
		}
	}

	ILispFunction& getValue() {
		return *dtype.tstruct.function.funcDesc->func;
	}
	using Lambda = typename ::Lambda;
	void call(ArgsList& args, shared_ptr<ICallResult> result) {
		if (isLambda()) {
			//reinterpret_cast<Lambda*>(this)->call(args, result);
			dtype.tstruct.function.funcDesc->lambdaDesc->lambda->call(args, result);
			return;
		}
		ILispFunction& fn = *(dtype.tstruct.function.funcDesc->func);
		fn.call(*getRunContext(), args, result);
		if (result->getStatus() != EResultStatus::success) {
			if (result->getStatus() == EResultStatus::unknown) {
				error("unknown operation status");
				return;
			}
			if (result->getStatus() == EResultStatus::error) {
				shared_ptr<IRunContext>& ctx = getGlobal().getRunContext();
				ErrorCallback& callback = ctx->getOnErrorCallback();
				callback(result->getLastError());
			}
			else {
				error("Not implemented handling for this operation status");
			}
			return;
		}
	}
};

class Lambda : public Function, public LambdaBase {
	PSexpr getParams() {
		return dtype.tstruct.function.funcDesc->lambdaDesc->params;
	}
	PSexpr getForms() {
		return dtype.tstruct.function.funcDesc->lambdaDesc->forms;
	}
public:
	Lambda(PSexpr params, PSexpr forms) : Function() {
		dtype.typeId = ETypeId::function;
		auto funcDesc = new FunctionDesc();
		funcDesc->type = EFunctionType::lambda;

		LambdaDesc* lambdaDesc = new LambdaDesc();
		lambdaDesc->params = params;
		lambdaDesc->forms = forms;
		lambdaDesc->lambda = this;

		funcDesc->lambdaDesc = lambdaDesc;
		dtype.tstruct.function.funcDesc = funcDesc;
	}
	~Lambda() {
		dtype.tstruct.function.funcDesc->lambdaDesc->lambda = nullptr;
		delete dtype.tstruct.function.funcDesc->lambdaDesc;
		if (dtype.tstruct.function.funcDesc != nullptr) {
			delete dtype.tstruct.function.funcDesc;
			dtype.tstruct.function.funcDesc = nullptr;
		}
	}

	PSexpr mergeParamsArgs(ArgsList& args, PSexpr params) {
		auto cons = std::static_pointer_cast<Cons>(params);
		auto diBuilder = getGlobal().getRunContext()->getDIBuilder();
		PSexpr res = diBuilder->createNil();
		for (int i = 0; i < args.size(); ++i) {
			auto car = cons->car();
			PSexpr curVarVal = diBuilder->createNil();
			curVarVal = diBuilder->createCons(args.get(i), curVarVal);
			curVarVal = diBuilder->createCons(car, curVarVal);
			res = diBuilder->createCons(curVarVal, res);
			cons = std::static_pointer_cast<Cons>(cons->cdr());
		}

		//getGlobal().getRunContext()->debugPrint(res);
		return res;
	}

	virtual void call(ArgsList& args, shared_ptr<ICallResult>& result) override {
		shared_ptr<IRunContext>& ctx = getRunContext();
		auto params = getParams();
		cout << endl;
		//getGlobal().getRunContext()->debugPrint(params);
		auto forms = getForms();
		cout << endl;
		//getGlobal().getRunContext()->debugPrint(forms);

		IDIBuilder* diBuilder = ctx->getDIBuilder();
		PSexpr paramsArgs = mergeParamsArgs(args, params);

		PSexpr nil = diBuilder->createNil();

		auto argsAndForms = diBuilder->createCons(paramsArgs, forms);
		auto letSym = ctx->getProgram()->getSymByName("let");
		auto letForm = diBuilder->createCons(letSym, argsAndForms);
		//ctx->debugPrint(letForm);
		auto formForEval = std::static_pointer_cast<Sexpr>(letForm);
		cout << endl << endl;
		//ctx->debugPrint(formForEval);
		ctx->evalForm(formForEval, result);
	}
};

class Symbol : public Atom {
public:
	Symbol(const gstring& name) {
		dtype.typeId = ETypeId::symbol;
		SymbolDesc*& symDesc = dtype.tstruct.symbol.symDesc;
		symDesc = new SymbolDesc;
		symDesc->name = name;
	}
	~Symbol() {
		delete dtype.tstruct.symbol.symDesc;
	}
	const gstring& getName() {
		return dtype.tstruct.symbol.symDesc->name;
	}
	shared_ptr<Sexpr> getValue() {
		return getGlobal().getRunContext()->getProgram()
			->getProgramContext()->getScope()->get(getName());
	}
	void setValue(shared_ptr<Sexpr> val) {
		// TODO!!! Fast finding cur Scope
		getGlobal().getRunContext()->getProgram()
			->getProgramContext()->getScope()->add(getName(), val);
	}
	shared_ptr<Sexpr> getFunction() {
		// TODO!!! Fast finding cur function Scope
		auto fnScope = getGlobal().getRunContext()->getProgram()->getProgramContext()->getFnScope();
		return fnScope->get(getName());
	}
	void setFunction(shared_ptr<Function> func) {
		// TODO!!! Fast finding cur function Scope
		auto fnScope = getGlobal().getRunContext()->getProgram()->getProgramContext()->getFnScope();
		fnScope->add(getName(), std::dynamic_pointer_cast<Sexpr>(func));
	}
	void setSelfEval() {
		dtype.tstruct.symbol.symDesc->bIsSelfEvaluated = true;
	}
	bool unsetSelfEval() {
		dtype.tstruct.symbol.symDesc->bIsSelfEvaluated = false;
	}
	bool getSelfEval() {
		return dtype.tstruct.symbol.symDesc->bIsSelfEvaluated;
	}
};
