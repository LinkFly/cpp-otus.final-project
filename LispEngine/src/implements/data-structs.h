#pragma once

#include <memory>
#include <tuple>

#include "../share.h"
#include "../interfaces/evaluator/IProgram.h"
#include "../interfaces/evaluator/IRunContext.h"
#include "../interfaces/evaluator/ICallResult.h"

#include "../interfaces/i-data-structs.h"

using std::unique_ptr;

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

struct SymbolDesc {
	gstring name;
	shared_ptr<Sexpr> bound;
	shared_ptr<Sexpr> fbound;
	bool bIsSelfEvaluated = false;
};

struct SymbolStruct {
	//unique_ptr<SymbolDesc> symDesc;
	SymbolDesc* symDesc;
};

struct CustomTypeStruct {
	CustomType* pCustom;
};

struct FunctionStruct {
	ILispFunction* func;
};

union TypeStruct {
	NilStruct nil;
	NumberStruct number;
	SymbolStruct symbol;
	ConsStruct cons;
	FunctionStruct function;
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
		return dtype.typeId == ETypeId::function;
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
	static shared_ptr<Cons>& AsCons(PSexpr& psexpr) {
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
	static shared_ptr<Number>& AsNumber(const PSexpr psexpr) {
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

class Function : public Atom {
	//shared_ptr<LispFunction> func;
	ILispFunction& getFunc() {
		return *dtype.tstruct.function.func;
	}
	IMemoryManager& getMemMan() {
		return getGlobal().getMemoryManager();
	}
	shared_ptr<IRunContext>& getRunContext() {
		auto global = getGlobal();
		return global.getRunContext();
	}
public:
	Function(shared_ptr<ILispFunction>& lispFunc) {
		dtype.typeId = ETypeId::function;
		dtype.tstruct.function.func = lispFunc.get();
		getMemMan().takeSharedPtrFunc(lispFunc.get(), lispFunc);
	}
	~Function() {
		getMemMan().freeSharedPtrFunc(dtype.tstruct.function.func);
	}

	ILispFunction& getValue() {
		return *dtype.tstruct.function.func;
	}

	void call(ArgsList& args, ICallResult& result) {
		ILispFunction& fn = *(dtype.tstruct.function.func);
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
		return dtype.tstruct.symbol.symDesc->bound;
	}
	shared_ptr<Sexpr> getFunction() {
		return dtype.tstruct.symbol.symDesc->fbound;
	}
	void setValue(shared_ptr<Sexpr> val) {
		dtype.tstruct.symbol.symDesc->bound = val;
	}
	void setFunction(shared_ptr<Function> func) {
		// TODO Checking function
		dtype.tstruct.symbol.symDesc->fbound = func;
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
