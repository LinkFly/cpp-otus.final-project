#pragma once

#include "share.h"

//

//#include "interfaces/evaluator/IProgram.h"
#include "interfaces/evaluator/IRunContext.h"
#include "interfaces/evaluator/ICallResult.h"

#include "implements/evaluator/Program.h"

class PlusLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		PSexpr arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1);
		auto oneArg = reinterpret_cast<Number*>(arg1.get());
		if (oneArg == nullptr) {
			auto error = make_shared<ErrorBadArg>();
			res.setErrorResult(error);
			return;
		}
		PSexpr arg2 = args.get(1);
		arg2 = evalArg(ctx, arg2);
		auto twoArg = reinterpret_cast<Number*>(arg2.get());
		if (oneArg == nullptr || twoArg == nullptr) {
			cerr << "Bad args\n";
			exit(-1);
		}
		//Sexpr* resNumber = new Number(oneArg->getValue() + twoArg->getValue());
		shared_ptr<Sexpr> resNumber = std::static_pointer_cast<Sexpr>(
			make_shared<Number>(oneArg->getValue() + twoArg->getValue()));
		res.setResult(resNumber, nullptr /*[resNumber]() {
			delete resNumber;
			}*/);
	}
	//asdf
};

class CarLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1);
		auto oneArg = arg1.get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res.setResult(oneArg->_getDType().tstruct.cons.pCell->car, nullptr);
	}
};

class CdrLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1);
		auto oneArg = arg1.get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res.setResult(oneArg->_getDType().tstruct.cons.pCell->cdr, nullptr);
	}
};

class ConsLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg);
		auto twoArg = args.get(1);
		twoArg = evalArg(ctx, twoArg);
		if (oneArg.get() == nullptr || twoArg.get() == nullptr) {
			cerr << "Bad args\n";
			exit(-1);
		}
		res.setResult(make_shared<Cons>(oneArg, twoArg), nullptr);
	}
};

class SetqLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0); // not evaluated
		auto twoArg = args.get(1);
		twoArg = evalArg(ctx, twoArg);
		if (!oneArg->isSymbol()) {
			cerr << "ERROR: first arg is not Symbol\n";
			exit(-1);
		}
		auto sym = std::static_pointer_cast<Symbol>(oneArg);
		sym->setValue(twoArg);
		res.setResult(twoArg, nullptr);
	}
};

class QuoteLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0);
		res.setResult(oneArg, nullptr);
	}
};

class EvalLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0);
		auto firstEvaluated = evalArg(ctx, oneArg);
		auto secondEvaluated = evalArg(ctx, firstEvaluated);
		res.setResult(secondEvaluated, nullptr);
	}
};

class IfLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg);
		bool bIsTrue = !oneArg->isNil();
		auto nextArg = args.get(bIsTrue ? 1 : 2);
		nextArg = evalArg(ctx, nextArg);
		res.setResult(nextArg, nullptr);
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
	void extendScope(IRunContext& ctx, shared_ptr<IScope>& scope, shared_ptr<Cons>& consVarVal) {
		//auto curVarVal = std::static_pointer_cast<Cons>(consVarVal->car());
		auto var = std::static_pointer_cast<Symbol>(consVarVal->car());
		auto tmp = std::static_pointer_cast<Cons>(consVarVal->cdr());
		auto val = tmp->car();
		val = evalArg
		(ctx, val);
		scope->add(var->getName(), val);
	}
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
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
				extendScope(ctx, scope, curVarVal);
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
				nextArg = evalArg(ctx, nextArg);
			}
			res.setResult(nextArg, nullptr);
			return;
		}
		// TODO use diBuilder
		res.setResult(make_shared<Nil>(), nullptr);
		// restore scope
		scope =  scope->popScope();
		setScope(ctx, scope);
	}
};

class LambdaLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, ICallResult& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg);
		bool bIsTrue = !oneArg->isNil();
		auto nextArg = args.get(bIsTrue ? 1 : 2);
		nextArg = evalArg(ctx, nextArg);
		res.setResult(nextArg, nullptr);
	}
};