#pragma once

#include "share.h"

#include <functional>
//#include <sstream>
#include <map>
#include <memory>

#include "StrTemplate.h"

using std::map;

enum class EErrorCode {
	noError, badArg, symbolNotExists, symbolUnbound
};

class Error : public CClass {
protected:
	gstring message;
	/*StrTemplate strTmpl;*/
public:
	EErrorCode code;
	Error(const gstring& message, EErrorCode code) : message{ message }, code{ code } {}

	static shared_ptr<Error>& getNoError() {
		static auto errNoError = make_shared<Error>("no error", EErrorCode::noError);
		return errNoError;
	}

	virtual gstring getMessage() {
		return message;
	}
};

class ErrorHandling : public CClass {
	void operator()(IRunContext& runContext, Error& error) {
		cout << error.getMessage() << endl;
	}
};

using ErrorCallback = std::function<void(shared_ptr<Error>& error)>;

class ErrorBadArg : public Error {
public:
	ErrorBadArg() : Error(gstring{ "Bad arg" }, EErrorCode::badArg) {}
	
};

class ErrorSymbolUnbound : public Error {
	unique_ptr<StrTemplate> strTemplate;
	gstring symName;
public:
	ErrorSymbolUnbound() : Error(gstring{ "Symbol `{{symName}}` is unbound" }, EErrorCode::symbolUnbound) {}
	ErrorSymbolUnbound(const gstring& symName) : ErrorSymbolUnbound{} {
		this->symName = symName;
		strTemplate.reset(new StrTemplate{ message });
	}
	virtual gstring getMessage() override {
		map<gstring, gstring> vars;
		vars["symName"] = symName;
		return strTemplate->str(vars);
	}
};
