#pragma once

#include "../../share.h"
#include "../DataStructs.h"

class IMemoryManager : public IInterface {
	/*virtual ICons createCell*/
};

class IProgram : public IInterface {
	virtual void constructor(IMemoryManager mman) = 0;
};