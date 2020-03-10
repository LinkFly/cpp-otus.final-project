#pragma once

#include "share.h"
//#include "interfaces/evaluator/IProgram.h"
#include "implements/evaluator/Program.h"

class PlusLispFunction : public LispFunction {
public:
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = reinterpret_cast<Number*>(args.get(0).get());
		auto twoArg = reinterpret_cast<Number*>(args.get(1).get());
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
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0).get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res.setResult(oneArg->_getDType().tstruct.cons.pCell->car, nullptr);
	}
};

class CdrLispFunction : public LispFunction {
public:
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0).get();
		if (oneArg == nullptr || !oneArg->isCons()) {
			cerr << "Bad argument\n";
			exit(-1);
		}
		res.setResult(oneArg->_getDType().tstruct.cons.pCell->cdr, nullptr);
	}
};

class ConsLispFunction : public LispFunction {
public:
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0);
		auto twoArg = args.get(1);
		if (oneArg.get() == nullptr || twoArg.get() == nullptr) {
			cerr << "Bad args\n";
			exit(-1);
		}
		res.setResult(make_shared<Cons>(oneArg, twoArg), nullptr);
	}
};

class SetqLispFunction : public LispFunction {
public:
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0);
		auto twoArg = args.get(1);
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
	void call(ArgsList& args, CallResult& res) override {
		auto oneArg = args.get(0);

		//if (!oneArg->isSymbol()) {
		//	cerr << "ERROR: first arg is not Symbol\n";
		//	exit(-1);
		//}
		//auto sym = std::static_pointer_cast<Symbol>(oneArg);
		//sym->setValue(twoArg);
		//res.setResult(twoArg, nullptr);
	}
};