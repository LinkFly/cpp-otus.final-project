#pragma once

#include "../build/configure.h"

#include <iostream>
#include <cstdint>
#include <memory>

#include "general-types.h"
#include "forward-declarations.h"

using std::cout;
using std::cerr;
using std::endl;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void error(const gstring& errMes, int errCode = -1) {
	cerr << errMes << endl;
	exit(errCode);
}

using PSexpr = shared_ptr<Sexpr>;

// Enums
enum class EResultStatus {
	unknown, success, error
};
