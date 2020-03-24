#pragma once

#include "../../../base/share.h"

//

#include "../../../interfaces/core/evaluator/IRunContext.h"

class RunContext : public IRunContext, public CClass {
	IEvaluator& evaluator;
	shared_ptr<IRunContext> createRunContext(shared_ptr<IRunContext>& parent) {
		shared_ptr<IRunContext> ctx =
			getGlobal().getIDIBuilder()->createRunContext(evaluator);
		ctx->setLevel(getLevel() + 1);
		ctx->setParentContext(parent);
		return ctx;
	}
	uint8_t level;
	shared_ptr<Error> error = Error::getNoError();

	ErrorCallback onErrorCallback;
	shared_ptr<IRunContext> parentCtx;
	
	IDIBuilder* diBuilder = nullptr;
	EResultStatus status = EResultStatus::success;

	shared_ptr<IProgram> program;
public:
	
	RunContext(IEvaluator& evaluator, IDIBuilder* diBuilder) : 
		evaluator{ evaluator }, level{ 0 }, diBuilder{ diBuilder } {
		program = diBuilder->createProgram(*this/**diBuilder*/);
	}

	virtual void setQuit() override {
		evaluator.setQuit();
	}

	//virtual void evalForm(PSexpr& sexpr, ICallResult& callRes) override {
	//	evaluator.evalForm(sexpr, callRes);
	//}

	virtual void evalForm(PSexpr& sexpr, shared_ptr<ICallResult>& callRes) override {
		evaluator.evalForm(sexpr, callRes);
	}

	virtual void evalSexprStr(gstring& sexprStr, ICallResult& callRes) override {
		evaluator.evalSexprStr(sexprStr, callRes);
	}

	virtual void evalSexprStr(gstring& sexprStr, shared_ptr<ICallResult>& callRes) override {
		evaluator.evalSexprStr(sexprStr, callRes);
	}

	virtual uint8_t getLevel() override {
		return level;
	}

	virtual void setLevel(uint8_t level) override {
		this->level = level;
	}

	virtual shared_ptr<IRunContext> pushNewContext(shared_ptr<IRunContext>& parent) override {
		return createRunContext(parent);
	}

	virtual shared_ptr<IRunContext> popContext() override {
		auto res = parentCtx;
		return res;
	}

	virtual void setOnErrorCallback(ErrorCallback callback) override {
		onErrorCallback = callback;
	}

	virtual ErrorCallback& getOnErrorCallback() override {
		return onErrorCallback;
	}
	/////////////////////////////////////
	virtual EResultStatus getStatus() override {
		return status;
	}
	virtual void setStatus(EResultStatus status) override {
		this->status = status;
	}
	virtual void setStatus(EResultStatus status, std::function<void()> deleter) override {

	}

	//virtual PSexpr getResult() override {

	//}
	//virtual void setResult(PSexpr result) override {

	//}
	virtual shared_ptr<Error>& getLastError() override {
		return error;
	}
	virtual void setLastError(shared_ptr<Error>& error) override {
		this->error = error;
	}


	////////////////////////////////////////////

	virtual shared_ptr<IRunContext> getParentContext() override {
		return std::static_pointer_cast<IRunContext>(parentCtx);
	}
	virtual void setParentContext(shared_ptr<IRunContext>& runCtx) override {
		parentCtx = runCtx;
	}

	//virtual shared_ptr<IRunContext>& getParentContext() override {
	//	return std::static_pointer_cast<IRunContext>(parentCtx);
	//}

	//virtual void setParentContext(shared_ptr<IRunContext>& runCtx) override {
	//	parentCtx = runCtx;
	//}

	//////////////////////////
	virtual IDIBuilder* getDIBuilder() override {
		return diBuilder;
	}

	virtual void setDIBuilder(IDIBuilder* diBuilder) override {
		this->diBuilder = diBuilder;
	}

	virtual shared_ptr<IProgram>& getProgram() override {
		return program;
	}

	virtual void setProgram(shared_ptr<IProgram>& program) override {
		this->program = program;
	}

	virtual bool isTopLevel() {
		return getParentContext().get() == nullptr;
	}

	void debugPrintAtom(PSexpr elt) {
		if (elt->isSymbol()) {
			auto sym = std::static_pointer_cast<Symbol>(elt);
			cout << " " << sym->getName();
		}
		else if (elt->isNil()) {
			cout << " nil";
		}
		else if (elt->isNumber()) {
			auto num = std::static_pointer_cast<Number>(elt);
			cout << " " << num->getValue();
		}
		else if (elt->isFunction()) {
		cout << " <function>";
		}
		else {
			cout << " <unknown>";
		}
	}

	void debugPrintNext(PSexpr head, PSexpr rest) {
		debugPrint(head);
		/*shared_ptr<Cons> cons = std::static_pointer_cast<Cons>(consSexpr);
		auto car = cons->car();
		auto cdr = cons->cdr();*/
		if (rest->isNil()) {
			cout << " )" << endl;
		}
		else if (rest->isAtom()) {
			cout << " .";
			debugPrintAtom(rest);
		}
		else if (rest->isCons()) {
			shared_ptr<Cons> cons = std::static_pointer_cast<Cons>(rest);
			auto car = cons->car();
			auto cdr = cons->cdr();
			debugPrintNext(car, cdr);
		}
	}
	virtual void debugPrint(PSexpr consSexpr) override {
		if (!consSexpr->isCons()) {
			debugPrintAtom(consSexpr);
		}
		else {
			cout << " ("; //<< endl << "\t";
			shared_ptr<Cons> cons = std::static_pointer_cast<Cons>(consSexpr);
			auto car = cons->car();
			auto cdr = cons->cdr();
			debugPrintNext(car, cdr);
		}
		//if (consSexpr->isCons()) {
		//	shared_ptr<Cons> cons = std::static_pointer_cast<Cons>(consSexpr);
		//	auto car = cons->car();
		//	auto cdr = cons->cdr();
		//	cout << "("; //<< endl << "\t";
		//	//auto curCons = std::static_pointer_cast<Cons>(car);
		//	debugPrint(car);
		//	if (cdr->isNil()) {
		//		cout << ")" << endl;
		//	}
		//	else {
		//		if (!cdr->isCons()) {
		//			cout << " . ";
		//			debugPrintAtom(cdr);
		//		}
		//		else if (cdr->isCons()) {
		//			shared_ptr<Cons> cons = std::static_pointer_cast<Cons>(cdr);
		//			auto curCar = cons->car();
		//			auto curCdr = cons->cdr();
		//			debugPrint(curCar);
		//			if (curCdr->isNil()) {
		//				cout << ")" << endl;
		//			}
		//			else {
		//				debugPrint(curCdr);
		//			}
		//		}
		//	}
		//}
		
		
		
		/*Printer printer;
		printer(cons);*/
	}
};