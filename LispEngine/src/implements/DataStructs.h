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

//class Atom : public IAtom, CClass {
//	shared_ptr<IValue> value;
//protected:
//	void constructor(shared_ptr<IValue> val) override {
//		value = val;
//	}
//public:
//	Atom() = default;
//
//	Atom(shared_ptr<IValue> val) {
//		constructor(val);
//	}
//	bool isAtom() override {
//		return true;
//	}
//	shared_ptr<IValue> getValue() override {
//		return value;
//	}
//};

//class Number : public Atom, public INumber {
//	/*NumberValue numVal;*/
//	bool isAtom() override {
//		return Atom::isAtom();
//	}
//
//	void constructor(shared_ptr<IValue> val) override {
//		Atom::constructor(val);
//	}
//	virtual shared_ptr<IValue> getValue() override {
//		return Atom::getValue();
//	}
//	/*virtual int32_t getNumber() = 0;*/
//
//	void constructor(int32_t num) override {
//		auto numVal = make_shared<NumberValue>(num);
//		Atom::constructor(numVal);
//	}
//public:
//	Number(int32_t num) {
//		constructor(num);
//	}
//	
//	int32_t getNumber() {
//		return dynamic_cast<NumberValue*>(Atom::getValue().get())->getNumber();
//	}
//};

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

//struct TypeBaseStruct {
//
//};
//
//struct NilStruct: TypeBaseStruct {
//	void* nil = nullptr;
//};

//struct NumberStruct: TypeBaseStruct {
//	int64_t num;
//};
//
//class CustomType: TypeBaseStruct {
//
//};
//
//struct Cell {
//
//};
//
//struct ConsStruct {
//	PSexpr _car;
//	PSexpr _cdr;
//};
//
//struct CustomTypeStruct {
//	CustomType* pCustom;
//};

class DynamicType: public CClass{
public:
	ETypeId typeId;
	Block block;
	//union TypeStruct {
	//	NilStruct nil{};
	//	NumberStruct num;
	//	CustomTypeStruct custom;
	//	TypeStruct() {};
	//} tstruct;
	//DynamicType() {};
};

class Type : public CClass {

};

class Sexpr : public CClass {
protected:
	DynamicType dtype;
};

class Atom : public Sexpr {

};



class Cons : public Sexpr {
	//using cons_t = tuple<Atom, Atom>;
	PSexpr _car;
	PSexpr _cdr;
public:
	Cons(shared_ptr<Sexpr>& car, shared_ptr<Sexpr>& cdr) : _car{ car }, _cdr{ cdr } {
		dtype.typeId = ETypeId::cons;
		/*dtype.block.init(sizeof(cons_t));
		auto cons = std::make_tuple(car, cdr);
		memcpy(dtype.block.ptr, &cons, sizeof(cons_t));*/
	}
	PSexpr car() {
		return _car;
		//return std::get<0>(*reinterpret_cast<cons_t*>(dtype.block.ptr));
	}
	PSexpr cdr() {
		return _cdr;
		//return std::get<1>(*reinterpret_cast<cons_t*>(dtype.block.ptr));
	}
	void car(PSexpr val) {
		_car = val;
		//std::get<0>(*reinterpret_cast<cons_t*>(dtype.block.ptr)) = val;
	}
	void cdr(PSexpr val) {
		_cdr = val;
		//std::get<1>(*reinterpret_cast<cons_t*>(dtype.block.ptr)) = val;
	}

	~Cons() {
		//cout << "=== Called Cons ===\n";
	}
};

class Number : public Atom {
	
public:
	Number() : Number(0) {}
	Number(int64_t val) {
		dtype.typeId = ETypeId::number;
		dtype.block.init(sizeof(val));
		memcpy(dtype.block.ptr, &val, sizeof(val));
	}
	int32_t getValue() {
		return *reinterpret_cast<int32_t*>(dtype.block.ptr);
	}
};

class PointerAtom : public Atom {
	DynamicType dtype;
public:
	PointerAtom(Atom* val) {
		dtype.typeId = ETypeId::pointerToAtom;
		dtype.block.ptr = reinterpret_cast<uint8_t*>(val);
	}
	Atom* getValue() {
		return reinterpret_cast<Atom*>(dtype.block.ptr);
	}
};

class PointerCons : public Atom {
public:
	PointerCons(Cons* val) {
		dtype.typeId = ETypeId::pointerToCons;
		dtype.block.ptr = reinterpret_cast<uint8_t*>(val);
	}
	Cons* getValue() {
		return reinterpret_cast<Cons*>(dtype.block.ptr);
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
public:
	Function(shared_ptr<LispFunction>& lispFunc) {
		dtype.typeId = ETypeId::function;
		func = lispFunc;
	}
	shared_ptr<LispFunction> getValue() {
		return func;
	}

	void call(ArgsList& args, CallResult& result) {
		getValue()->call(args, result);
	}
};

class Symbol : public Atom {
	struct SymbolStruct {
		gstring name;
		shared_ptr<Sexpr> bound;
		shared_ptr<Sexpr> fbound;
	};
	unique_ptr<SymbolStruct> symData;
public:
	Symbol(gstring& name) {
		dtype.typeId = ETypeId::symbol;
		symData = make_unique<SymbolStruct>();
		symData->name = name;
	}
	shared_ptr<Sexpr> getValue() {
		return symData->bound;
	}
	shared_ptr<Sexpr> getFunction() {
		return symData->fbound;
	}
	void setValue(shared_ptr<Sexpr> val) {
		symData->bound = val;
	}
	void setFunction(shared_ptr<Function> func) {
		// TODO Checking function 
		symData->fbound = func;
	}
};



//class Cons : public ICons, CClass {
//	shared_ptr<std::tuple<IAtom, IAtom>> cell;
//public:
//	bool isAtom() override {
//		return false;
//	}
//	IAtom& car() override {
//		return std::get<0>(*cell);
//	}
//	virtual IAtom& cdr() override {
//		return std::get<1>(*cell);
//	}
//	void setCar(IAtom& val) override {
//		std::get<0>(*cell) = val;
//	}
//	void setCdr(IAtom& val) override {
//		std::get<1>(*cell) = val;
//	}
//};