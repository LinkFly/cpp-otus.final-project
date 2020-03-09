#pragma once

#include "../build/configure.h"

#include <string>
#include <iostream>
#include <cstdint>
#include <memory>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;
using std::make_unique;

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

void error(const string& errMes, int errCode = -1) {
	cerr << errMes << endl;
	exit(errCode);
}

using short_size = uint8_t;
using Cid = uint16_t;
using gstring = std::string;
using gchar = char;

class IMemoryManager;
class MemoryManager;
class DIBuilder;
class Sexpr;
class LispFunction;
class ArgsList;
class CallResult;
using PSexpr = shared_ptr<Sexpr>;
