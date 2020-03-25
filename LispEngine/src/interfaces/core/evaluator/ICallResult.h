#pragma once

#include "../../../base/share.h"

class ICallResult : public IInterface {
public:
	virtual PSexpr& getResult() = 0;
	virtual void setResult(PSexpr result) = 0;
	virtual void setResult(PSexpr sexpr, std::function<void()> deleter = nullptr) = 0;
	virtual shared_ptr<Error> getLastError() = 0;
	virtual void setLastError(shared_ptr<Error> err) = 0;
	virtual void setErrorResult(shared_ptr<Error> error, std::function<void()> deleter = nullptr) = 0;
	virtual EResultStatus getStatus() = 0;
	virtual void setStatus(EResultStatus status) = 0;
	virtual void operator=(ICallResult& callRes) = 0;
	virtual Number& getNumberResult() = 0;
	virtual ~ICallResult() {

	}
};