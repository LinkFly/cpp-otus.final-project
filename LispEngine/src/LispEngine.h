#pragma once

#include "share.h"
#include "interfaces/DataStructs.h"

#include "implements/DiBuilder.h"
#include "interfaces/DataStructs.h"
#include "interfaces/evaluator/IScope.h"

#include "implements/evaluator/Program.h"

class LispEngine : public CClass {
	DIBuilder diBuilder;
	IDIBuilder& idiBuilder = *(dynamic_cast<IDIBuilder*>(&diBuilder));
	shared_ptr<IReader> reader = diBuilder.createReader(idiBuilder);
	shared_ptr<IScope> scope = diBuilder.createScope();
	shared_ptr<IProgram> program = diBuilder.createProgram(idiBuilder);
	shared_ptr<IEvaluator> evaluator = diBuilder.createEvaluator();

	PSexpr createSymbol(gstring& symName) {
		PSexpr symSexpr = diBuilder.createSymbol(symName);
		scope->add(symName, symSexpr);
		return symSexpr;
	}

	PSexpr& getSymbol(gstring& symName) {
		return scope->get(symName);
	}
public:
	LispEngine() {
		program->getProgramContext()->setScope(scope);

		gstring symName = "plus";
		PSexpr symSexpr = createSymbol(symName);

		shared_ptr<LispFunction> plusLispFunction = diBuilder.create<PlusLispFunction>();
		shared_ptr<Function> plusFunction = diBuilder.createFunction(plusLispFunction);

		shared_ptr<SetfSymbolFunction> setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();
		PSexpr plusFuncSexpr = std::dynamic_pointer_cast<Sexpr>(plusFunction);
		ArgsList args(symSexpr, plusFuncSexpr);
		CallResult callRes;
		setfSymbolFunction->call(args, callRes);
	}

	void readProgram(gstring& sProgram) {
		reader->read(sProgram, *program.get());
	}

	void evalProgram() {
		evaluator->eval(*program.get());
	}

	template<class ConcreteSexpr>
	ConcreteSexpr& getLastResult() {
		return evaluator->getLastResult().getResult<ConcreteSexpr>();
	}
};

