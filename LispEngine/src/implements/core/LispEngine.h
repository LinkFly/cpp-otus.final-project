#pragma once

#include "share.h"

#include "../DiBuilder.h"
#include "../../interfaces/core/i-data-structs.h"
#include "../../interfaces/core/evaluator/IScope.h"
#include "../../interfaces/core/evaluator/IProgram.h"
#include "../../interfaces/core/ILispEngine.h"

#include "evaluator/Program.h"
#include "evaluator/BaseFunctions.h"
#include "Printer.h"

class LispEngine : public ILispEngine, public CClass, public LispEngineBase {
	DIBuilder diBuilder;
	Global global;
	IDIBuilder& idiBuilder;
	shared_ptr<IReader> reader;
	shared_ptr<IScope> scope;
	shared_ptr<IScope> fnScope;
	shared_ptr<IEvaluator> evaluator;
	shared_ptr<SetfSymbolFunction> setfSymbolFunction;
	shared_ptr<Nil> nil;
	shared_ptr<Printer> printer;
	shared_ptr<Repl> repl;
	ErrorCallback errCallback;

	bool isQuit = false;
public:
	LispEngine() : idiBuilder{ *(dynamic_cast<IDIBuilder*>(&diBuilder)) } {
		setLispEngine(dynamic_cast<LispEngineBase*>(this));
		global = Global{};
		getGlobal().setIDIBuilder(&diBuilder);
		evaluator = diBuilder.createEvaluator(this);
		getGlobal().setEvaluator(evaluator);

		reader = diBuilder.createReader();
		scope = diBuilder.createScope();
		fnScope = diBuilder.createScope();

		setfSymbolFunction = diBuilder.create<SetfSymbolFunction>();
		nil = diBuilder.createNil();
		printer = diBuilder.createPrinter();
		repl = diBuilder.createRepl(*this);
		errCallback;

		
		//initGlobal();
		//auto fn = [this]() {
		//	return global;
		//};
		//setFnGlobal(&fn);
		/*setGlobal(&global);*/

		

		

		initTopLevelRunContext();
		errCallback = [](shared_ptr<Error>& err) {
			cerr << err->getMessage() << endl;
		};
		getGlobal().getTopLevelRunContext()->setOnErrorCallback(errCallback);

		/*repl = diBuilder.createRepl(*this);*/
		getGlobal().getTopLevelRunContext()->getProgram()->getProgramContext()->setScope(scope);
		getGlobal().getTopLevelRunContext()->getProgram()->getProgramContext()->setFnScope(fnScope);

		registerLispFunction<PlusLispFunction>("plus");
		registerLispFunction<PlusLispFunction>("+");
		registerLispFunction<MinusLispFunction>("-");
		registerLispFunction<MultipleLispFunction>("*");
		registerLispFunction<DivideLispFunction>("/");

		registerLispFunction<CarLispFunction>("car");
		registerLispFunction<CdrLispFunction>("cdr");
		registerLispFunction<ConsLispFunction>("cons");
		registerLispFunction<SetqLispFunction>("setq");
		registerLispFunction<QuoteLispFunction>("quote");
		registerLispFunction<IfLispFunction>("if");
		registerLispFunction<LetLispFunction>("let");
		registerLispFunction<EvalLispFunction>("eval");
		registerLispFunction<LambdaLispFunction>("lambda");
		registerLispFunction<ApplyLispFunction>("apply");
		registerLispFunction<LoadLispFunction>("load");
		registerLispFunction<QuitLispFunction>("quit");

		// Predicates
		registerLispFunction<SymbolpLispFunction>("symbolp");
		registerLispFunction<AtompLispFunction>("atomp");
		registerLispFunction<ConspLispFunction>("consp");
		registerLispFunction<NumberpLispFunction>("numberp");
		registerLispFunction<FunctionpLispFunction>("functionp");
		registerLispFunction<StringpLispFunction>("stringp");
		//////

		registerLispFunction<TagbodyLispFunction>("tagbody");
		registerLispFunction<PrintLispFunction>("print");

		registerSymbolSelfEvaluated("t");
		registerSymbolSelfEvaluated("nil");
		registerSymbolValue("nil", std::static_pointer_cast<Sexpr>(nil));

	}
	
	virtual void setQuit() override {
		isQuit = true;
	}

	virtual bool getQuit() override {
		return isQuit;
	}

	ErrorCallback& getErrorCallback() {
		return errCallback;
	}
	PSexpr createSymbol(const gstring& symName) {
		//PSexpr symSexpr = diBuilder.createSymbol(symName);
		//program->getProgramContext()->getScope()->add(symName, symSexpr);
		//return symSexpr;
		PSexpr& symSexpr = getProgram()->createSymbol(symName);
		return symSexpr;
	}

	PSexpr& getSymbol(gstring& symName) {
		/*return scope->get(symName);*/
		return getProgram()->getProgramContext()->getScope()->get(symName);
	}

	PSexpr getSymbolValue(const gstring& symName) {
		/*return scope->get(symName);*/
		PSexpr value = std::static_pointer_cast<Symbol>(
			getProgram()->getProgramContext()->getScope()->get(symName));
		return value;
	}

	template<class TConcreteFunction>
	void registerLispFunction(const gstring& symName) {
		PSexpr& symSexpr = createSymbol(symName);

		shared_ptr<ILispFunction> plusLispFunction = diBuilder.createLispFunction<TConcreteFunction>();
		shared_ptr<Function> plusFunction = diBuilder.createFunction(plusLispFunction);


		PSexpr plusFuncSexpr = std::static_pointer_cast<Sexpr>(plusFunction);
		ArgsList args(symSexpr, plusFuncSexpr);
		shared_ptr<ICallResult> callRes = diBuilder.createCallResult();
		setfSymbolFunction->call(*getGlobal().getTopLevelRunContext(), args, callRes);
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
	}

	void initTopLevelRunContext() {
		shared_ptr<IRunContext> ctx = diBuilder.createRunContext(*evaluator);
		ctx->setDIBuilder(&diBuilder);
		getGlobal().setTopLevelRunContext(ctx);
	}

	static unique_ptr<Global>& getPGlobal() {
		static unique_ptr<Global> global;
		return global;
	}
	/* Global* global = nullptr;*/

	static void initGlobal() {
		/*if (getGlobalController().getGlobal().get() != nullptr) {
			string err = "ERROR: Global already initialized";
			error(err);
		}*/
		/*if (global != nullptr) {
			string err = "ERROR: Global already initialized";
			error(err);
		}*/
		/*global = new Global();*/
		/*auto pGlobal = make_unique<Global>();
		getPGlobal().swap(pGlobal);*/
		/*auto ptrGlobal = make_unique<Global>();
		getGlobalController().setGlobal(ptrGlobal);*/

		createGlobal();
	}
	static void clean() {
		/*getGlobalController().resetGlobal();*/
		freeGlobal();
	}
	~LispEngine() {
		/*delete global;
		global = nullptr;*/
		/*getPGlobal().reset();*/

		//clean();
		//setGlobal(nullptr);
	}
	virtual Global& getGlobal() override {
		return global;
	}
	
	void readProgram(gstring& sProgram) {
		reader->read(sProgram, *getProgram().get());
	}

	PSexpr parseSymbol(const gstring& sSym) {
		return reader->parseSymbol(sSym, *getProgram());
	}

	shared_ptr<IProgram> getProgram() {
		auto& ctx = getGlobal().getRunContext();
		if (ctx.get() == nullptr) {
			return getGlobal().getTopLevelRunContext()->getProgram();
		}
		return ctx->getProgram();
	}

	void evalProgram(shared_ptr<ICallResult> callRes = nullptr) {
		auto& program = getProgram();
		evaluator->eval(*program.get(), callRes, [this](shared_ptr<Error>& err) {
			createRunContext(true);

			repl->printError(err);
			auto& ctx = getGlobal().getRunContext();
			ctx->setLastError(err);

			repl->run(ctx);
			});
	}
	//void evalProgram() {
	//	
	//}

	template<class ConcreteSexpr>
	ConcreteSexpr& getLastResult() {
		auto result = evaluator->getLastResult().getResult();
		return *reinterpret_cast<ConcreteSexpr*>(result.get());
	}

	virtual PSexpr getLastPSexprRes() override {
		return evaluator->getLastResult().getResult();
	}

	virtual shared_ptr<ICallResult> getLastCallResult() override {
		return evaluator->getPLastResult();
	}

	gstring princ(PSexpr& res) {
		return (*printer)(res);
	}

	void createRunContext(bool isNewDebugLevel) {
		evaluator->createRunContext(scope, fnScope, isNewDebugLevel);
	}

	void evalSexprStr(gstring& sexprStr) override {
		createRunContext(false);
		readProgram(sexprStr);
		evalProgram();
	}

	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult> callRes) override {
		createRunContext(false);
		readProgram(sexprStr);
		evalProgram(callRes);
	}

	virtual Printer getPrinter() override {
		return *printer;
	}

	void runRepl() {
		createRunContext(true);
		/*repl->run(getGlobal().getTopLevelRunContext());*/
		repl->run(getGlobal().getRunContext());
	}
};
