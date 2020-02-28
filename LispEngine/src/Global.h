#pragma once

#include "share-base.h"

enum class ETypeId { pointer, number, symbol, function, cons };

class Global {
	// meta handling ETypeId
	static const short_size typeIdBits = 4;
};