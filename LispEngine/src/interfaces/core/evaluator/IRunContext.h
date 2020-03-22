#pragma once

#include "../../../base/share.h"

#include <functional>

#include "../../IDIBuilder.h"
#include "../ILispEngine.h"

class IRunContext : public IInterface {
public:
	virtual void setQuit() = 0;
	/*virtual void evalForm(PSexpr& sexpr, ICallResult& callRes) = 0;*/
	virtual void evalForm(PSexpr& sexpr, shared_ptr<ICallResult>& callRes) = 0;
	virtual void evalSexprStr(gstring& sexprStr, ICallResult& callRes) = 0;
	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult>& callRes) = 0;
	virtual uint8_t getLevel() = 0;
	virtual void setLevel(uint8_t level) = 0;
	virtual shared_ptr<IRunContext> pushNewContext(shared_ptr<IRunContext>& parent) = 0;
	virtual shared_ptr<IRunContext> popContext() = 0;
	virtual void setOnErrorCallback(ErrorCallback callback) = 0;
	virtual ErrorCallback& getOnErrorCallback() = 0;

	virtual EResultStatus getStatus() = 0;
	virtual void setStatus(EResultStatus status) = 0;
	virtual void setStatus(EResultStatus status, std::function<void()> deleter) = 0;
	//virtual PSexpr getResult() = 0;
	//virtual void setResult(PSexpr result) = 0;
	virtual shared_ptr<Error>& getLastError() = 0;
	virtual void setLastError(shared_ptr<Error> & error) = 0;

	virtual shared_ptr<IRunContext> getParentContext() = 0;
	virtual void setParentContext(shared_ptr<IRunContext>& runCtx) = 0;

	///////////////
	virtual IDIBuilder* getDIBuilder() = 0;
	virtual void setDIBuilder(IDIBuilder* diBuilder) = 0;

	virtual shared_ptr<IProgram>& getProgram() = 0;
	virtual void setProgram(shared_ptr<IProgram>& diBuilder) = 0;

	virtual bool isTopLevel() = 0;

	virtual void debugPrint(PSexpr cons) = 0;
};