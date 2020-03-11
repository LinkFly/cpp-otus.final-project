#pragma once

#include "share.h"
#include "interfaces/DataStructs.h"

#include "implements/DiBuilder.h"
#include "interfaces/DataStructs.h"
#include "interfaces/evaluator/IScope.h"
#include "interfaces/evaluator/IProgram.h"

#include "implements/evaluator/Program.h"
#include "BaseFunctions.h"

class LispEngine : public CClass {
	DIBuilder diBuilder;
	IDIBuilder& idiBuilder = *(dynamic_cast<IDIBuilder*>(&diBuilder));
	shared_ptr<IReader> reader = diBuilder.createReader(idiBuilder);
	shared_ptr<IScope> scope = diBuilder.createScope();
	shared_ptr<IProgram> program = diBuilder.createProgram(idiBuilder);
	shared_ptr<IEvaluator> evaluator = diBuilder.createEvaluator();
	shared_ptr<IEvaluator> topLevelEvaluator = diBuilder.createEvaluator();
	shared_ptr<SetfSymbolFunction> setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();

public:
	PSexpr& createSymbol(const gstring& symName) {
		//PSexpr symSexpr = diBuilder.createSymbol(symName);
		//program->getProgramContext()->getScope()->add(symName, symSexpr);
		//return symSexpr;
		return program->createSymbol(symName);
	}

	PSexpr& getSymbol(gstring& symName) {
		/*return scope->get(symName);*/
		return program->getProgramContext()->getScope()->get(symName);
	}

	PSexpr& getSymbolValue(const gstring& symName) {
		/*return scope->get(symName);*/
		auto sym = std::static_pointer_cast<Symbol>(
			program->getProgramContext()->getScope()->get(symName));
		return sym->getValue();
	}

	template<class TConcreteFunction>
	void registerLispFunction(const gstring& symName) {
		PSexpr symSexpr = createSymbol(symName);

		shared_ptr<ILispFunction> plusLispFunction = diBuilder.create<TConcreteFunction>();
		shared_ptr<Function> plusFunction = diBuilder.createFunction(plusLispFunction);


		PSexpr plusFuncSexpr = std::static_pointer_cast<Sexpr>(plusFunction);
		ArgsList args(symSexpr, plusFuncSexpr);
		CallResult callRes;
		setfSymbolFunction->call(*getGlobal().getTopLevelRunContext(), args, callRes);
	}

	void registerSymbolValue(const gstring& symName, PSexpr& value) {
		PSexpr symSexpr = createSymbol(symName);
		shared_ptr<Symbol> sym = std::static_pointer_cast<Symbol>(symSexpr);
		sym->setValue(value);
	}

	void initTopLevelRunContext() {
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*topLevelEvaluator);
		getGlobal().setTopLevelRunContext(ctx);
	}

	LispEngine() {
		program->getProgramContext()->setScope(scope);
		registerLispFunction<PlusLispFunction>("plus");
		registerLispFunction<CarLispFunction>("car");
		registerLispFunction<CdrLispFunction>("cdr");
		registerLispFunction<ConsLispFunction>("cons");
		registerLispFunction<SetqLispFunction>("setq");
		registerLispFunction<QuoteLispFunction>("quote");
		//////

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

	PSexpr& getLastPSexprRes() {
		return evaluator->getLastResult().result;
	}
};

