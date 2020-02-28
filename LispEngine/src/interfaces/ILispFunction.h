#pragma once

#include "../share.h"
#include "../interfaces/DataStructs.h"

class IException : public IInterface {

}

class IExceptionList : public IInterface {

}

class ICallResult : public IInterface {
	virtual bool isSuccess() = 0;
	virtual ISexpr getResult() = 0;
	virtual IExceptionList getExceptions() = 0;
}

class IArgsList : public IInterface {
	virtual ISexpr get(short_size argPos) = 0;
};

class ILispFunction: public IInterface {
	virtual void constructor(IProgram program) = 0;
	virtual ICallResult call(IArgsList args) = 0;
};