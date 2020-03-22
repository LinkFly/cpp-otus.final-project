#pragma once

#include "share.h"
#include "../../../interfaces/core/i-data-structs.h"

#include "../../../implements/DiBuilder.h"
#include "../../../implements/core/evaluator/Program.h"

class Core : public CClass {
	DIBuilder diBuilder;
public:
	Core() {
		gstring symName = "plus";
		PSexpr symSexpr = diBuilder.createSymbol(symName);

		shared_ptr<LispFunction> plusLispFunction = diBuilder.create<PlusLispFunction>();
		shared_ptr<Function> plusFunction = diBuilder.createFunction(plusLispFunction);

		shared_ptr<SetfSymbolFunction> setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();
		CallResult callRes;
		/*PSexpr symSexpr = std::dynamic_pointer_cast<Sexpr>(sym);
		shared_ptr<Sexpr> symSexpr2 = std::dynamic_pointer_cast<Sexpr>(sym);;*/
		PSexpr plusFuncSexpr = std::dynamic_pointer_cast<Sexpr>(plusFunction);
		ArgsList args(symSexpr, plusFuncSexpr);
		setfSymbolFunction->call(args, callRes);
	}
};