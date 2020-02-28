#pragma once

#include <memory>
#include <tuple>

#include "../share.h"
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

class Atom : public IAtom, CClass {
	shared_ptr<IValue> value;
protected:
	void constructor(shared_ptr<IValue> val) override {
		value = val;
	}
public:
	Atom() = default;

	Atom(shared_ptr<IValue> val) {
		constructor(val);
	}
	bool isAtom() override {
		return true;
	}
	shared_ptr<IValue> getValue() override {
		return value;
	}
};

class Number : public Atom, public INumber {
	/*NumberValue numVal;*/
	bool isAtom() override {
		return Atom::isAtom();
	}

	void constructor(shared_ptr<IValue> val) override {
		Atom::constructor(val);
	}
	virtual shared_ptr<IValue> getValue() override {
		return Atom::getValue();
	}
	/*virtual int32_t getNumber() = 0;*/

	void constructor(int32_t num) override {
		auto numVal = make_shared<NumberValue>(num);
		Atom::constructor(numVal);
	}
public:
	Number(int32_t num) {
		constructor(num);
	}
	
	int32_t getNumber() {
		return dynamic_cast<NumberValue*>(Atom::getValue().get())->getNumber();
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