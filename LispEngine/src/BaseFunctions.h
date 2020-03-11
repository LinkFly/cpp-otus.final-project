#pragma once

#include "share.h"
//#include "interfaces/evaluator/IProgram.h"
#include "implements/evaluator/Program.h"

class PlusLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
		PSexpr arg1 = args.get(0);
		arg1 = evalArg(ctx, arg1);
		auto oneArg = reinterpret_cast<Number*>(arg1.get());
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
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
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
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
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
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
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
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
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
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0);
		res.setResult(oneArg, nullptr);
	}
};

class IfLispFunction : public LispFunction {
public:
	void call(IRunContext& ctx, ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0);
		oneArg = evalArg(ctx, oneArg);
		bool bIsTrue = !oneArg->isNil();
		auto nextArg = args.get(bIsTrue ? 1 : 2);
		nextArg = evalArg(ctx, nextArg);
		res.setResult(nextArg, nullptr);
	}
};