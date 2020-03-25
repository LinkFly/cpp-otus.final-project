#pragma once

#include "../../../base/share.h"

//

#include "../../../interfaces/core/evaluator/ICallResult.h"

// Warning! Maybe memory leak
class CallResult : public ICallResult, public CClass {
	template< class To, class From >
	inline std::shared_ptr< To > dynamic_pointer_cast(std::shared_ptr< From >&& ptr) {
		To* newPtr = reinterpret_cast<To*>(ptr.release());
		return std::shared_ptr< To >((Number*)(ptr));
	}

	std::function<void()> deleter = nullptr;
	//std::vector<shared_ptr<Error>> errors;
	shared_ptr<Error> lastError;
	EResultStatus status = EResultStatus::unknown;
public:
	PSexpr result;

	virtual void setResult(PSexpr sexpr) override {
		result = sexpr;
	}

	virtual shared_ptr<Error> getLastError() override {
		return lastError;
	}
	virtual void setLastError(shared_ptr<Error> err) override {
		lastError = err;
	}

	void setResult(PSexpr sexpr, std::function<void()> deleter = nullptr) override {
		status = EResultStatus::success;
		result = sexpr;
		this->deleter = deleter;
	}

	void setErrorResult(shared_ptr<Error> error, std::function<void()> deleter = nullptr) override {
		status = EResultStatus::error;
		setLastError(error);
		this->deleter = deleter;
	}

	template<class ConcreteSexpr>
	ConcreteSexpr& getConcreteResult() {
		return *reinterpret_cast<ConcreteSexpr*>(result.get());
	}

	virtual PSexpr& getResult() override {
		return result;
	}

	~CallResult() {
		if (deleter != nullptr) {
			deleter();
		}
	}

	virtual EResultStatus getStatus() override {
		return status;
	}
	virtual void setStatus(EResultStatus status) override {
		this->status = status;
	}

	virtual void operator=(ICallResult& callRes) override {
		status = callRes.getStatus();
		lastError = callRes.getLastError();
	}

	virtual Number& getNumberResult() override {
		return getConcreteResult<Number>();
	}
};