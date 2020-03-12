#pragma once

#include "share.h"
#include "interfaces/i-data-structs.h"

#include "implements/DiBuilder.h"
#include "interfaces/i-data-structs.h"
#include "interfaces/evaluator/IScope.h"
#include "interfaces/evaluator/IProgram.h"
#include "interfaces/ILispEngine.h"

#include "implements/evaluator/Program.h"
#include "BaseFunctions.h"
#include "Printer.h"

class LispEngine : public ILispEngine, public CClass {
	DIBuilder diBuilder;
	IDIBuilder& idiBuilder = *(dynamic_cast<IDIBuilder*>(&diBuilder));
	shared_ptr<IReader> reader = diBuilder.createReader(idiBuilder);
	shared_ptr<IScope> scope = diBuilder.createScope();
	shared_ptr<IProgram> program = diBuilder.createProgram(idiBuilder);
	shared_ptr<IEvaluator> evaluator = diBuilder.createEvaluator();
	shared_ptr<IEvaluator> topLevelEvaluator = diBuilder.createEvaluator();
	shared_ptr<SetfSymbolFunction> setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();
	shared_ptr<Nil> nil = diBuilder.createNil();
	shared_ptr<Printer> printer = diBuilder.createPrinter();
	shared_ptr<Repl> repl = diBuilder.createRepl(*this);
	ErrorCallback errCallback;

public:
	ErrorCallback& getErrorCallback() {
		return errCallback;
	}
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
		shared_ptr<ICallResult> callRes = diBuilder.createCallResult();
		setfSymbolFunction->call(*getGlobal().getTopLevelRunContext(), args, *callRes);
	}

	void registerSymbolValue(const gstring& symName, PSexpr& value) {
		PSexpr symSexpr = createSymbol(symName);
		shared_ptr<Symbol> sym = std::static_pointer_cast<Symbol>(symSexpr);
		sym->setValue(value);
	}

	void registerSymbolSelfEvaluated(const gstring& symName) {
		PSexpr symSexpr = createSymbol(symName);
		shared_ptr<Symbol> sym = std::static_pointer_cast<Symbol>(symSexpr);
		sym->setSelfEval();
		/*PSexpr symSexpr = createSymbol(symName);
		shared_ptr<Symbol> sym = std::static_pointer_cast<Symbol>(symSexpr);
		sym->setValue(symSexpr);*/
		//shared_ptr<Nil> pnil = diBuilder.createNil();
		////auto nil = *pnil;
		//registerSymbolValue(symName, *pnil);
		//PSexpr& symSexpr = createSymbol(symName);
		//std::static_pointer_cast<Symbol>(symSexpr)->setValue(symSexpr);
		////sym->setValue(symSexpr);
	}

	void initTopLevelRunContext() {
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*topLevelEvaluator);
		ctx->setDIBuilder(&diBuilder);
		getGlobal().setTopLevelRunContext(ctx);
	}

	LispEngine() {
		/*repl = diBuilder.createRepl(*this);*/
		program->getProgramContext()->setScope(scope);
		registerLispFunction<PlusLispFunction>("plus");
		registerLispFunction<CarLispFunction>("car");
		registerLispFunction<CdrLispFunction>("cdr");
		registerLispFunction<ConsLispFunction>("cons");
		registerLispFunction<SetqLispFunction>("setq");
		registerLispFunction<QuoteLispFunction>("quote");
		registerLispFunction<IfLispFunction>("if");
		registerLispFunction<LetLispFunction>("let");
		//////

		registerSymbolSelfEvaluated("t");
		registerSymbolValue("nil", std::static_pointer_cast<Sexpr>(nil));

		errCallback = [](Error& err) {
			cerr << err.message << endl;
		};

		initTopLevelRunContext();
		getGlobal().getTopLevelRunContext()->setOnErrorCallback(errCallback);
		
	}
	void readProgram(gstring& sProgram) {
		reader->read(sProgram, *program.get());
	}

	PSexpr parseSymbol(const gstring& sSym) {
		return reader->parseSymbol(sSym, *program);
	}

	void evalProgram() {
		evaluator->eval(*program.get(), [](Error& err) {
			cerr << "[ERROR]: " << err.message << endl;
			});
	}

	template<class ConcreteSexpr>
	ConcreteSexpr& getLastResult() {
		auto result = evaluator->getLastResult().getResult();
		return *reinterpret_cast<ConcreteSexpr*>(result.get());
	}

	PSexpr& getLastPSexprRes() {
		return evaluator->getLastResult().getResult();
	}

	gstring evalSexprStr(gstring& sexprStr) override {
		readProgram(sexprStr);
		evalProgram();
		return (*printer)(getLastPSexprRes());
	}

	void runRepl() {
		repl->run(getGlobal().getTopLevelRunContext());
	}
};

