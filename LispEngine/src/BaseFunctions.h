#pragma once

#include "share.h"

//
#include <vector>
#include <map>

//#include "interfaces/evaluator/IProgram.h"
#include "interfaces/evaluator/IRunContext.h"
#include "interfaces/evaluator/ICallResult.h"

#include "implements/evaluator/Program.h"
#include "Printer.h"
#include "System.h"

using std::map;
using std::vector;

class ArithmBaseLispFunction : public LispFunction {
public:
	virtual shared_ptr<Number> arithmOperation(Number* one, Number* two) = 0;

	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		PSexpr arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto oneArg = reinterpret_cast<Number*>(arg1.get());

		PSexpr arg2 = args.get(1);
		arg2 = evalArg(ctx, arg2, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto twoArg = reinterpret_cast<Number*>(arg2.get());

		shared_ptr<Sexpr> resNumber = std::static_pointer_cast<Sexpr>(arithmOperation(oneArg, twoArg));
		/*shared_ptr<Sexpr> resNumber = std::static_pointer_cast<Sexpr>(
			make_shared<Number>(oneArg->getValue() + twoArg->getValue()));*/
		res->setResult(resNumber, nullptr);
	}
};

class PlusLispFunction : public ArithmBaseLispFunction {
public:
	virtual shared_ptr<Number> arithmOperation(Number* oneArg, Number* twoArg) override {
		return make_shared<Number>(oneArg->getValue() + twoArg->getValue());
	}
};

class MinusLispFunction : public ArithmBaseLispFunction {
public:
	virtual shared_ptr<Number> arithmOperation(Number* oneArg, Number* twoArg) override {
		return make_shared<Number>(oneArg->getValue() - twoArg->getValue());
	}
};

class MultipleLispFunction : public ArithmBaseLispFunction {
public:
	virtual shared_ptr<Number> arithmOperation(Number* oneArg, Number* twoArg) override {
		return make_shared<Number>(oneArg->getValue() * twoArg->getValue());
	}
};

class DivideLispFunction : public ArithmBaseLispFunction {
public:
	virtual shared_ptr<Number> arithmOperation(Number* oneArg, Number* twoArg) override {
		return make_shared<Number>(oneArg->getValue() / twoArg->getValue());
	}
};

class CarLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto oneArg = arg1.get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res->setResult(oneArg->_getDType().tstruct.cons.pCell->car, nullptr);
	}
};

class CdrLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto oneArg = arg1.get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res->setResult(oneArg->_getDType().tstruct.cons.pCell->cdr, nullptr);
	}
};

class ConsLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto twoArg = args.get(1);
		twoArg = evalArg(ctx, twoArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		if (oneArg.get() == nullptr || twoArg.get() == nullptr) {
			cerr << "Bad args\n";
			exit(-1);
		}
		res->setResult(make_shared<Cons>(oneArg, twoArg), nullptr);
	}
};

class SetqLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0); // not evaluated
		auto twoArg = args.get(1);
		twoArg = evalArg(ctx, twoArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		if (!oneArg->isSymbol()) {
			cerr << "ERROR: first arg is not Symbol\n";
			exit(-1);
		}
		auto sym = std::static_pointer_cast<Symbol>(oneArg);
		sym->setValue(twoArg);
		res->setResult(twoArg, nullptr);
	}
};

class QuoteLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0);
		res->setResult(oneArg, nullptr);
	}
};

class EvalLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0);
		auto firstEvaluated = evalArg(ctx, oneArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto secondEvaluated = evalArg(ctx, firstEvaluated, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		res->setResult(secondEvaluated, nullptr);
	}
};

class IfLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		bool bIsTrue = !oneArg->isNil();
		auto nextArg = args.get(bIsTrue ? 1 : 2);
		nextArg = evalArg(ctx, nextArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		res->setResult(nextArg, nullptr);
	}
};

class LetLispFunction : public LispFunction {
	shared_ptr<IScope> getScope(IRunContext& ctx) {
		return ctx.getProgram()->getProgramContext()->getScope();
	}
	void setScope(IRunContext& ctx, shared_ptr<IScope>& scope) {
		return ctx.getProgram()->getProgramContext()->setScope(scope);
	}
	//void addToScope(IRunContext& ctx, gstring& symName, PSexpr& val) {
	//	shared_ptr<IScope>& scope = getScope(ctx);
	//	scope->add(symName, val);
	//}
	void extendScope(IRunContext& ctx, shared_ptr<IScope>& scope, shared_ptr<Cons>& consVarVal,
		shared_ptr<ICallResult>& res) {
		//auto curVarVal = std::static_pointer_cast<Cons>(consVarVal->car());
		auto var = std::static_pointer_cast<Symbol>(consVarVal->car());
		auto tmp = std::static_pointer_cast<Cons>(consVarVal->cdr());
		auto val = tmp->car();
		val = evalArg(ctx, val, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		scope->add(var->getName(), val);
	}
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto oneArg = args.get(0); // not evaluted
		shared_ptr<IScope> scope = getScope(ctx);
		if (oneArg->isCons()) {
			// Create and set new scope
			scope = scope->pushNewScope(scope);
			setScope(ctx, scope);

			auto& cons = std::static_pointer_cast<Cons>(oneArg);
			//PSexpr curVarVal;
			do {
				auto& curVarVal = std::static_pointer_cast<Cons>(cons->car());
				//shared_ptr<Cons> curVarVal;
				extendScope(ctx, scope, curVarVal, res);
				if (res->getStatus() != EResultStatus::success) {
					return;
				}
				//curVarVal->
				cons = std::static_pointer_cast<Cons>(cons->cdr());
			} while (!cons->isNil());
		}
		else {
			if (!oneArg->isNil()) {
				error("ERROR: Bad LET arg");
			}
		}
		// push new scope
		// ...
		// working with scope
		// ...
		if (args.size() > 1) {
			PSexpr nextArg;
			for (short_size i = 1; i < args.size(); ++i) {
				nextArg = args.get(i);
				nextArg = evalArg(ctx, nextArg, res);
				if (res->getStatus() != EResultStatus::success) {
					return;
				}
			}
			res->setResult(nextArg, nullptr);
			return;
		}
		// TODO use diBuilder
		res->setResult(make_shared<Nil>(), nullptr);
		// restore scope
		scope =  scope->popScope();
		setScope(ctx, scope);
	}
};

class LambdaLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto params = args.get(0);
		auto diBuilder = ctx.getDIBuilder();
		PSexpr forms = std::static_pointer_cast<Sexpr>(diBuilder->createNil());
		if (args.size() > 1) {
			for (int i = 1; i < args.size(); ++i) {
				auto curForm = args.get(i);
				forms = diBuilder->createCons(curForm, forms);
			}
		}
		PSexpr lambdaFunction = diBuilder->createLambda(params, forms);
		res->setResult(lambdaFunction, nullptr);
	}
};

class ApplyLispFunction: public LispFunction{
	shared_ptr<ArgsList> getArgsList(PSexpr args) {
		shared_ptr<ArgsList> argsList = make_shared<ArgsList>();
		PSexpr curArgs = args;
		while (!curArgs->isNil() && curArgs->isCons()) {
			auto cons = std::static_pointer_cast<Cons>(curArgs);
			auto car = cons->car();
			curArgs = cons->cdr();
			argsList->addArg(car);
		}
		return argsList;
	}
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto symOrFunc = args.get(0);
		//ctx.debugPrint(symOrFunc);
		symOrFunc = evalArg(ctx, symOrFunc, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		auto argsForFn = args.get(1);
		//ctx.debugPrint(argsForFn);
		argsForFn = evalArg(ctx, argsForFn, res);
		//ctx.debugPrint(argsForFn);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		PSexpr funcSexpr;
		if (symOrFunc->isSymbol()) {
			auto sym = std::static_pointer_cast<Symbol>(symOrFunc);
			funcSexpr = sym->getFunction();
		}
		else {
			funcSexpr = symOrFunc;
		}
		auto func = std::static_pointer_cast<Function>(funcSexpr);
		
		//auto diBuilder = ctx.getDIBuilder();
		//auto callRes = diBuilder->createCallResult();
		shared_ptr<ArgsList> argsList = getArgsList(argsForFn);
		func->call(*argsList, res);
	}
};

class LoadLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto fstArg = args.get(0);
		auto file = evalArg(ctx, fstArg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		if (!file->isString()) {
			auto err = make_shared< ErrorArgNotStr>();
			res->setErrorResult(err, nullptr);
		}
		auto str = std::static_pointer_cast<String>(file);
		auto filePath = str->getValue();
		gstring fileContent;
		bool bReaded = System::readFile(filePath, fileContent);
		if (!bReaded) {
			auto err = make_shared<ErrorBadFileReaded>();
			res->setErrorResult(err, nullptr);
			return;
		}
		auto diBuilder = ctx.getDIBuilder();
		//auto callRes = diBuilder->createCallResult();
		ctx.evalSexprStr(fileContent, res);
		/*res->setStatus(callRes->getStatus());
		res->*/
		/*res = *callRes;*/
	}
};

class QuitLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto diBuilder = ctx.getDIBuilder();
		auto t = ctx.getProgram()->getProgramContext()->getScope()->find("t");
		//PSexpr t = ctx.getProgram()->getSymByName("t");
		ctx.setQuit();
		res->setResult(t, nullptr);
	}
};

class PredicateBaseLispFunction : public LispFunction {
protected:
	virtual bool checkSepxr(PSexpr& sexpr) = 0;
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		/*auto diBuilder = ctx.getDIBuilder();*/
		auto arg = args.get(0);
		arg = evalArg(ctx, arg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		bool checkRes = checkSepxr(arg);
		PSexpr resSexpr = ctx.getProgram()->getProgramContext()->getScope()->find(checkRes ? "t" : "nil");
		res->setResult(resSexpr, nullptr);
	}
};

class SymbolpLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isSymbol();
	}
};

class AtompLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isAtom();
	}
};

class ConspLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isCons();
	}
};

class NumberpLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isNumber();
	}
};

class FunctionpLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isFunction();
	}
};

class StringpLispFunction : public PredicateBaseLispFunction {
	virtual bool checkSepxr(PSexpr& sexpr) override {
		return sexpr->isString();
	}
};

class TagbodyLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto diBuilder = ctx.getDIBuilder();
		auto nil = diBuilder->createNil();
		res->setResult(nil, nullptr);

		vector<PSexpr> forms;
		map<gstring, size_t> points;
		bool isHandleNext = false;
		bool isWasPoint = false;
		gstring curPoint;
		for (int i = 0; i < args.size(); ++i) {
			auto curArg = args.get(i);
			if (curArg->isCons()) {
				/*if (!isWasPoint) {
					evalArg(ctx, curArg, res);
					if (res->getStatus() != EResultStatus::success) {
						return;
					}
				}
				else {*/
				if (isHandleNext) {
					forms.push_back(curArg);
					auto curIdx = forms.size() - 1;
					points[curPoint] = curIdx;
					isHandleNext = false;
				}
				else {
					forms.push_back(curArg);
				}
				//}

			}
			else if (curArg->isSymbol()) {
				/*isWasPoint = true;*/
				isHandleNext = true;
				auto sym = std::static_pointer_cast<Symbol>(curArg);
				curPoint = sym->getName();
			}
		}
		///////// Evaluating saved forms //////////
		size_t idx = 0;
		gstring goSymName = "go";
		while (true) {
			if (idx >= forms.size())
				break;
			auto curForm = forms[idx];
			auto cons = std::static_pointer_cast<Cons>(curForm);
			auto car = cons->car();
			if (car->isSymbol()) {
				auto sym = std::static_pointer_cast<Symbol>(car);
				if (sym->getName() == goSymName) {
					auto cdrCons = std::static_pointer_cast<Cons>(cons->cdr());
					auto symPoint = std::static_pointer_cast<Symbol>(cdrCons->car());
					idx = points[symPoint->getName()];
					continue;
				}
			}
			evalArg(ctx, curForm, res);
			if (res->getStatus() != EResultStatus::success) {
				return;
			}
			++idx;
		}

		auto t = ctx.getProgram()->getProgramContext()->getScope()->find("nil");
		res->setResult(t, nullptr);
	}
};

class PrintLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, shared_ptr<ICallResult>& res) override {
		auto arg = args.get(0);
		arg = evalArg(ctx, arg, res);
		if (res->getStatus() != EResultStatus::success) {
			return;
		}
		// TODO!!! Getting printer from context;
		Printer printer{};
		gstring resStr = printer(arg);
		cout << resStr << endl;
		//auto diBuilder = ctx.getDIBuilder();
		auto t = ctx.getProgram()->getProgramContext()->getScope()->find("t");
		res->setResult(t, nullptr);
	}
};