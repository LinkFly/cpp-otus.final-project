#pragma once

#include "../share.h"

#include <tuple>
#include <memory>


using std::tuple;
using std::shared_ptr;

class IValueData: public IInterface {
public:
	virtual short_size getCountBits() = 0;

	virtual ~IValueData() {};
};

//class INumberValue : public IValueData {
//	virtual void constructor
//};

class IValue : public IInterface {
	virtual void setValueComponents(ETypeId typeId, shared_ptr<IValueData> valueData) = 0;
	virtual tuple<ETypeId, shared_ptr<IValueData>> getValueComponents() = 0;
	
};

class ISexpr : public IInterface {
	virtual bool isAtom() = 0;
	
};

class IAtom : public ISexpr {
	virtual void constructor(shared_ptr<IValue> val) = 0;
	virtual shared_ptr<IValue> getValue() = 0;
};

class INumber : IInterface {
public:
	virtual void constructor(int32_t) = 0;
	virtual int32_t getNumber() = 0;
};


class ICons : public ISexpr {
	virtual IAtom& car() = 0;
	virtual IAtom& cdr() = 0;
	virtual void setCar(IAtom& val) = 0;
	virtual void setCdr(IAtom& val) = 0;
};

class ILispFunction;
class Cell;
