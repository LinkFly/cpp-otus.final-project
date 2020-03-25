#pragma once

#include "../../base/share.h"

#include <iostream>
#include <string>

#include "../../interfaces/core/ILispEngine.h"
#include "../../interfaces/core/IRepl.h"
#include "../../interfaces/core/evaluator/i-program-structs.h"

using std::cin;
using std::cout;

class Repl : public IRepl, public CClass {
	gstring prefix = "> ";
	ILispEngine& lispEngine;
public:
	Repl(ILispEngine& lispEngine) : lispEngine{ lispEngine } {}
	void handler(gstring& expr) {
		lispEngine.evalSexprStr(expr);
		auto res = lispEngine.getPrinter()(lispEngine.getLastPSexprRes());
		cout << res << endl;
	}
	virtual void printError(shared_ptr<Error>& err) override {
		cerr << "\n[ERROR]: " << err->getMessage() << endl;
	}
	virtual void run(shared_ptr<IRunContext> & ctx) override {
		outCommandPrompt(*ctx);
		while (true) {
			gstring cmd;
			std::getline(cin, cmd);
			//cin >> cmd;
			handler(cmd);
			if (lispEngine.getQuit()) {
				break;
			}
			outCommandPrompt(*ctx);
		}
	}
	void outCommandPrompt(IRunContext& ctx) {
		auto level = ctx.getLevel();
		gstring extraPrefix = "";
		if (level > 0) {
			extraPrefix = "[" + std::to_string(level) + "] ";
		}
		cout << "\n" << extraPrefix << prefix;
	}

};