#pragma once

#include <memory>
#include <tuple>

#include "../share.h"
#include "../interfaces/evaluator/IProgram.h"
#include "../interfaces/DataStructs.h"

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
};

struct SymbolStruct {
	//unique_ptr<SymbolDesc> symDesc;
	SymbolDesc* symDesc;
};

struct CustomTypeStruct {
	CustomType* pCustom;
};

struct FunctionStruct {
	LispFunction* func;
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
};

struct Cell {
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
	Cons* getValue() {
		return nullptr;
	}
};

class Function : public Atom {
	shared_ptr<LispFunction> func;
	LispFunction& getFunc() {
		return *dtype.tstruct.function.func;
	}
	IMemoryManager& getMemMan() {
		return getGlobal().getMemoryManager();
	}
public:
	Function(shared_ptr<LispFunction>& lispFunc) {
		dtype.typeId = ETypeId::function;
		dtype.tstruct.function.func = lispFunc.get();
		getMemMan().takeSharedPtrFunc(lispFunc.get(), lispFunc);
	}
	~Function() {
		getMemMan().freeSharedPtrFunc(dtype.tstruct.function.func);
	}

	LispFunction& getValue() {
		return *dtype.tstruct.function.func;
	}

	void call(ArgsList& args, CallResult& result) {
		LispFunction& fn = *(dtype.tstruct.function.func);
		fn.call(args, result);
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
};
