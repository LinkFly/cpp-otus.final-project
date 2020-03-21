#include <iostream>
#include <memory>

#include "Arguments.h"
#include "LispEngine.h"

using std::cout;
using std::endl;

int main(int argc, char** argv) {
	LispEngine lisp;

	Arguments arguments{};
	arguments.parse(argc, argv);
	if (arguments.file != "") {
		lisp.evalSexprStr("(load \"" + arguments.file + "\")");
	}
	if (arguments.sexprStr != "") {
		lisp.evalSexprStr(arguments.sexprStr);
	}
	if (arguments.file == "" && arguments.sexprStr == "") {
		lisp.runRepl();
	}
}