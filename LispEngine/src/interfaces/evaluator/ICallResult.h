#pragma once

#include "../../share.h"

class ICallResult : public IInterface {
	//template< class To, class From >
	//inline std::shared_ptr< To > dynamic_pointer_cast(std::shared_ptr< From >&& ptr) {
	//	To* newPtr = reinterpret_cast<To*>(ptr.release());
	//	return std::shared_ptr< To >((Number*)(ptr));
	//}

	//std::function<void()> deleter = nullptr;
	////std::vector<shared_ptr<Error>> errors;
	//shared_ptr<Error> lastError;
public:
	//virtual EResultStatus getStatus() = 0;
	//virtual void setStatus(EResultStatus status) = 0;
	///*EResultStatus status = EResultStatus::unknown;
	//PSexpr result;*/
	//virtual PSexpr getResult() = 0;
	virtual PSexpr& getResult() = 0;
	virtual void setResult(PSexpr result) = 0;
	virtual void setResult(PSexpr sexpr, std::function<void()> deleter = nullptr) = 0;
	//	status = EResultStatus::success;
	//	result = sexpr;
	//	this->deleter = deleter;
	//}

	virtual shared_ptr<Error> getLastError() = 0;
	virtual void setLastError(shared_ptr<Error> err) = 0;

	virtual void setErrorResult(shared_ptr<Error> error, std::function<void()> deleter = nullptr) = 0;/*{
		status = EResultStatus::error;
		lastError = error;
		this->deleter = deleter;
	}*/

	virtual EResultStatus getStatus() = 0;
	virtual void setStatus(EResultStatus status) = 0;

	//template<class ConcreteSexpr>
	//ConcreteSexpr& getResult() {
	//	//return *reinterpret_cast<ConcreteSexpr*>(result_ptr);
	//	return *reinterpret_cast<ConcreteSexpr*>(result.get());
	//}
	virtual ~ICallResult() {
		/*if (deleter != nullptr) {
			deleter();
		}*/
	}

	virtual void operator=(ICallResult& callRes) = 0;
	
	virtual Number& getNumberResult() = 0;
};