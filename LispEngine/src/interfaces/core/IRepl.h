#pragma once

#include "../../base/share.h"

class IRepl : public IInterface {
public:
	virtual void run(shared_ptr<IRunContext>& ctx) = 0;
	virtual void printError(shared_ptr<Error>& err) = 0;
};