#pragma once

#include "share.h"

#include <functional>

enum class EErrorCode {
	noError, badArg, symbolNotExists
};

class Error : public CClass {
public:
	gstring message;
	EErrorCode code;
	Error(const gstring& message, EErrorCode code) : message{ message }, code{ code } {}

	static shared_ptr<Error>& getNoError() {
		static auto errNoError = make_shared<Error>("no error", EErrorCode::noError);
		return errNoError;
	}
};

class ErrorHandling : public CClass {
	void operator()(IRunContext& runContext, Error& error) {
		cout << error.message << endl;

	}
};

class ErrorBadArg : public Error {
public:
	ErrorBadArg() : Error(gstring{ "Bad arg" }, EErrorCode::badArg) {}
};

using ErrorCallback = std::function<void(Error & error)>;