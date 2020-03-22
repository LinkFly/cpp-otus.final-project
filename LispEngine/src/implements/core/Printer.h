#pragma once

#include "share.h"

#include <string>

#include "data-structs.h"


class Printer : public CClass {
public:
	gstring operator()(PSexpr& sexpr) {
		if (sexpr->isNumber()) {
			// don't work
			/*auto num = Number::AsNumber(sexpr)->getValue();*/
			Number* numSexpr = static_cast<Number*>(sexpr.get());
			if (numSexpr == nullptr) {
				cerr << "Bad number" << endl;
				exit(-1);
			}
			auto num = numSexpr->getValue();
			return std::to_string(num);
		}
		if (sexpr->isNil()) {
			return "nil";
		}
		if (sexpr->isCons()) {
			auto pCell = sexpr->_getDType().tstruct.cons.pCell;
			auto car = pCell->car;
			auto cdr = pCell->cdr;
			return gstring{ "(" } +this->operator()(car) + " . " + this->operator()(cdr) + ")";
		}
		if (sexpr->isFunction()) {
			return "<function>";
		}
		if (sexpr->isSymbol()) {
			auto sym = std::static_pointer_cast<Symbol>(sexpr);
			if (sym->getSelfEval()) {
				return sym->getName();
			}
			return "<unbound symbol `" + sym->getName() + "`>";
		}
		if (sexpr->isString()) {
			auto str = std::static_pointer_cast<String>(sexpr);
			return gstring{ "\"" } + str->getValue() + gstring{ "\"" };
		}
		return "<unknown>";
	}
};