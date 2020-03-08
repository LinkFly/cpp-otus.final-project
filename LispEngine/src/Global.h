#pragma once

#include "share-base.h"

enum class ETypeId { nil, pointerToAtom, pointerToCons, number, symbol, function, cons, custom };

class Global {
	// meta handling ETypeId
	static const short_size typeIdBits = 4;
};