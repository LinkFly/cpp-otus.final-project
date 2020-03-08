#pragma once

#include "../../share.h"
#include "IProgram.h"

class IReader : public IInterface {
public:
	virtual void read(gstring& programText, IProgram& program) = 0;
};