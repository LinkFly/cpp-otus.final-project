#pragma once

#include "share.h"

#include <iostream>
#include <string>

#include "interfaces/ILispEngine.h"
#include "interfaces/evaluator/IProgram.h"

using std::cin;
using std::cout;

class Repl : public CClass {
	gstring prefix = "> ";
	ILispEngine& lispEngine;
public:
	Repl(ILispEngine& lispEngine) : lispEngine{ lispEngine } {}
	void handler(gstring& expr) {
		auto res = lispEngine.evalSexprStr(expr);
		cout << res << endl;
	}
	void run(shared_ptr<IRunContext> & ctx) {
		outCommandPrompt(*ctx);
		while (true) {
			gstring cmd;
			std::getline(cin, cmd);
			//cin >> cmd;
			handler(cmd);
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