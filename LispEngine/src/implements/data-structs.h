#pragma once

#include <memory>
#include <tuple>

#include "../share.h"
#include "../interfaces/evaluator/IProgram.h"
#include "../interfaces/evaluator/IRunContext.h"
#include "../interfaces/evaluator/ICallResult.h"
#include "../interfaces/IDIBuilder.h"

#include "../interfaces/i-data-structs.h"

using std::unique_ptr;

////////////////////////////////////////// Experimental /////////////////////////////////
class NumberData : public IValueData, CClass {
	int32_t num;
	short_size getCountBits() override {
		return sizeof(num) * 8;
	}
public:
	NumberData(int32_t numVal) {
		num = numVal;
	}
	int32_t getNumber() { return num; }
};

class Value : public IValue, CClass {
protected:
	ETypeId typeId;
	shared_ptr<IValueData> data;
public:
	void setValueComponents(ETypeId typeId, shared_ptr<IValueData> valueData) override {
		this->typeId = typeId;
		this->data = valueData;
	}
	tuple<ETypeId, shared_ptr<IValueData>> getValueComponents() override {
		return make_tuple(typeId, data);
	}
};

class NumberValue : public Value {
public:
	NumberValue(): NumberValue(0) {}
	NumberValue(int32_t numVal) {
		auto data = make_shared<NumberData>(numVal);
		setValueComponents(ETypeId::number, data);
	}
	int32_t getNumber() {
		auto pair = getValueComponents();
		return dynamic_cast<NumberData*>(std::get<1>(pair).get())->getNumber();
	}
};

class Block: public CClass {
public:
	uint8_t* ptr = nullptr;
	void init(size_t size) {
		if (ptr != nullptr) {
			delete[] ptr;
		}
		ptr = new uint8_t[size];
	}
	~Block() {
		delete[] ptr;
	}


};
////////////////////// end Experimental /////////////////////


///////////////////////////////////////////////////

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

struct NilStruct: TypeBaseStruct {
	void* nil = nullptr;
};

struct NumberStruct: TypeBaseStruct {
	int64_t num;
	int64_t operator=(int64_t val) {
		return num = val;
	}
	operator int64_t() {
		return num;
	}
};

class CustomType: TypeBaseStruct {

};

struct ConsStruct {
	Cell* pCell;
};

//struct SymValGetter {
//	//shared_ptr<Symbol> symbol;
//	//SymValGetter(shared_ptr<Symbol>& sym) {
//	//	symbol = sym;
//	//}
//	PSexpr get() {
//		return getGlobal().getRunContext()->getProgram()
//			->getProgramContext()->getScope()->get(symbol->getName());
//	}
//};

struct SymbolDesc {
	gstring name;
	shared_ptr<Sexpr> bound;
	shared_ptr<Sexpr> fbound;
	bool bIsSelfEvaluated = false;
};

class Cons;

class LambdaBase : public CClass {
public:
	virtual void call(ArgsList& args, ICallResult& callRes) = 0;
};

struct LambdaDesc {
	PSexpr params;
	PSexpr forms;
	//std::function<void(ArgsList&, ICallResult&)> fn;
	LambdaBase* lambda;
};

struct SymbolStruct {
	//unique_ptr<SymbolDesc> symDesc;
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

struct FunctionStruct {
	//ILispFunction* func;
	FunctionDesc* funcDesc;
};

//struct LambdaStruct {
//	LambdaDesc* lambdaDesc;
//};

union TypeStruct {
	NilStruct nil;
	NumberStruct number;
	SymbolStruct symbol;
	ConsStruct cons;
	FunctionStruct function;
	/*LambdaStruct lambda;*/
	CustomTypeStruct custom;
	public:
		TypeStruct() {};
		~TypeStruct() {};
};

class DynamicType: public CClass{
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
	void call(ArgsList& args, ICallResult& result) {
		if (isLambda()) {
			//reinterpret_cast<Lambda*>(this)->call(args, result);
			dtype.tstruct.function.funcDesc->lambdaDesc->lambda->call(args, result);
			return;
		}
		ILispFunction& fn = *(dtype.tstruct.function.funcDesc->func);
		fn.call(*getRunContext(), args, result);
		if (result.getStatus() != EResultStatus::success) {
			if (result.getStatus() == EResultStatus::unknown) {
				error("unknown operation status");
				return;
			}
			if (result.getStatus() == EResultStatus::error) {
				shared_ptr<IRunContext>& ctx = getGlobal().getRunContext();
				ErrorCallback& callback = ctx->getOnErrorCallback();
				callback(result.getLastError());
			}
			else {
				error("Not implemented handling for this operation status");
			}
			return;
		}
	}

	/*void call(PSexpr& args, ICallResult& result) {
		ArgsList argsList;
		PSexpr curArgs = args;
		while (!curArgs->isNil() && curArgs->isCons()) {
			auto cons = std::static_pointer_cast<Cons>(curArgs);
			auto car = cons->car();
			auto curArgs = cons->cdr();
			argsList.addArg(car);
		}
		call(argsList, result);
	}*/
};

//class ArgsList;

class Lambda : public Function, public LambdaBase {
	PSexpr getParams() {
		return dtype.tstruct.function.funcDesc->lambdaDesc->params;
	}
	PSexpr getForms() {
		return dtype.tstruct.function.funcDesc->lambdaDesc->forms;
	}
public:
	Lambda(PSexpr params, PSexpr forms): Function() {
		dtype.typeId = ETypeId::function;
		/*if (dtype.tstruct.function.funcDesc != nullptr) {
			delete dtype.tstruct.function.funcDesc;
			dtype.tstruct.function.funcDesc = nullptr;
		}*/
		auto funcDesc = new FunctionDesc();
		//auto funcDesc = dtype.tstruct.function.funcDesc;
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
	//using IDIBuilder = typename ::IDIBuilder;
	/*class IDIBuilder;*/
	
	//PSexpr mergeParamsArgs(PSexpr params, ArgsList& args) {
	//	auto cons = std::static_pointer_cast<Cons>(params);
	//	auto diBuilder = getGlobal().getRunContext()->getDIBuilder();
	//	PSexpr res = diBuilder->createNil();
	//	for (int i = 0; i < args.size(); ++i) {
	//		auto car = cons->car();
	//		auto curVarVal = diBuilder->createCons(car, args.get(i));
	//		res = diBuilder->createCons(curVarVal, res);
	//		cons = cons->cdr();
	//	}
	//	
	//	getGlobal().getRunContext()->debugPrint(res);
	//	return res;
	//}

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
		
		getGlobal().getRunContext()->debugPrint(res);
		return res;
	}

	virtual void call(ArgsList& args, ICallResult& result) override {
		shared_ptr<IRunContext>& ctx = getRunContext();
		auto params = getParams();
		cout << endl;
		getGlobal().getRunContext()->debugPrint(params);
		auto forms = getForms();
		cout << endl;
		getGlobal().getRunContext()->debugPrint(forms);

		//getGlobal().evaluator->

		IDIBuilder* diBuilder = ctx->getDIBuilder();
		PSexpr paramsArgs = mergeParamsArgs(args, params);
		//PSexpr paramsArgs = diBuilder->createNil();

		/*if (args.size() > 0) {
			for (int i = 0; i < args.size(); ++i) {

			}
		}*/

		PSexpr nil = diBuilder->createNil();
		//auto wrapListArgs = diBuilder->createCons(paramsArgs, nil);
		auto argsAndForms = diBuilder->createCons(paramsArgs, forms);
		auto letSym = ctx->getProgram()->getSymByName("let");
		auto letForm = diBuilder->createCons(letSym, argsAndForms);
		//ctx->debugPrint(letForm);
		auto formForEval = std::static_pointer_cast<Sexpr>(letForm);
		cout << endl << endl;
		getGlobal().getRunContext()->debugPrint(formForEval);
		ctx->evalForm(formForEval, result);
	}

	//shared_ptr<cons> reverse(shared_ptr<cons> list) {
	//	shared_ptr<iruncontext>& ctx = getruncontext();
	//	auto dibuilder = ctx->getdibuilder();
	//	psexpr paramsargs = dibuilder->createnil();
	//	while (true) {
	//		psexpr car = list->car();
	//		paramsargs = dibuilder->createcons(car, paramsargs);
	//		list = list->car();
	//		if (list->isnil()) {
	//			break;
	//		}
	//	}
	//	return std::static_pointer_cast<cons>(paramsargs);
	//}
	//class ConsLispFunction;
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
		/*return dtype.tstruct.symbol.symDesc->bound;*/
		return getGlobal().getRunContext()->getProgram()
			->getProgramContext()->getScope()->get(getName());
	}
	void setValue(shared_ptr<Sexpr> val) {
		/*dtype.tstruct.symbol.symDesc->bound = val;*/
		getGlobal().getRunContext()->getProgram()
			->getProgramContext()->getScope()->add(getName(), val);
	}
	shared_ptr<Sexpr> getFunction() {
		/*return dtype.tstruct.symbol.symDesc->fbound;*/
		auto fnScope = getGlobal().getRunContext()->getProgram()->getProgramContext()->getFnScope();
		return fnScope->get(getName());
	}
	void setFunction(shared_ptr<Function> func) {
		// TODO Checking function
		/*dtype.tstruct.symbol.symDesc->fbound = func;*/
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
