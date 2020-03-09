#pragma once

#include "share.h"
#include "interfaces/DataStructs.h"

#include "implements/DiBuilder.h"
#include "interfaces/DataStructs.h"
#include "interfaces/evaluator/IScope.h"
#include "interfaces/evaluator/IProgram.h"

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
		program->getProgramContext()->getScope()->add(symName, symSexpr);
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
		//cout << "plusLispFunction(plusLispFunction): " << plusLispFunction.get() << endl;
		shared_ptr<Function> plusFunction = diBuilder.createFunction(plusLispFunction);

		shared_ptr<SetfSymbolFunction> setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();
		PSexpr plusFuncSexpr = std::static_pointer_cast<Sexpr>(plusFunction);
		ArgsList args(symSexpr, plusFuncSexpr);
		CallResult callRes;
		setfSymbolFunction->call(args, callRes);
		/*auto curFnInScope = _getFn();
		PlusLispFunction& plusFn = *((PlusLispFunction*)(_getFnFromSym(symSexpr)));
		cout << "plusLispFunction(fromSym): " << plusLispFunction.get() << endl;
		cout << "plusLispFunction(LispEngine): " << plusLispFunction.get() << endl;
		cout << "plusLispFunction(inScope): " << _getFn() << endl;

		PlusLispFunction& plusFn2 = *((PlusLispFunction*)(_getFn()));
		cout << "plusLispFunction(as PlusLispFunction): " << &plusFn2 << endl;*/
	}
	void* _getFnFromSym(PSexpr& sexpr) {
		return sexpr->_getDType().tstruct.symbol.symDesc->fbound->_getDType().tstruct.function.func;
	}
	void* _getFn() {
		auto scope = program->getProgramContext()->getScope();
		gstring name = "plus";
		PSexpr& sexpr = scope->get(name);
		return _getFnFromSym(sexpr);
	}
	void readProgram(gstring& sProgram) {
		reader->read(sProgram, *program.get());
	}

	PSexpr parseSymbol(const gstring& sSym) {
		return reader->parseSymbol(sSym, *program);
	}

	void evalProgram() {
		evaluator->eval(*program.get());
	}

	template<class ConcreteSexpr>
	ConcreteSexpr& getLastResult() {
		return evaluator->getLastResult().getResult<ConcreteSexpr>();
	}
};

