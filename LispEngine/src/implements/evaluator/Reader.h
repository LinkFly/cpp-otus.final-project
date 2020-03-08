#pragma once

#include <map>
#include <functional>
#include <vector>

#include "../../share.h"
#include "../../interfaces/evaluator/IReader.h"
#include "../../interfaces/evaluator/IProgram.h"
//#include "../DiBuilder.h"
#include "../../interfaces/IDIBuilder.h"

using std::map;
using std::function;
using std::vector;

enum class Ops {
	startList, endList, startNumber, startSymbol
};

enum class LikeType {
	unknown, list, symbol, number
};

struct GCharDiap {
	gchar start;
	gchar end;
	GCharDiap(gchar start, gchar end = 0) : start{ start } {
		if (end == 0) {
			this->end = start;
		}
		else {
			this->end = end;
		}
	}
};
class ReadTable: public CClass {
public:
	//map<GCharDiap, LikeType> map = map<GCharDiap, LikeType>{
	//	{GCharDiap(gchar{'('}, gchar{'('}), LikeType::list},
	//	{GCharDiap(gchar{')'}, gchar{')'}), LikeType::list},
	//	{GCharDiap(gchar{'0'}, gchar{'9'}), LikeType::number},
	//	{GCharDiap(gchar{'a'}, gchar{'z'}), LikeType::symbol},
	//	{GCharDiap(gchar{'+'}, gchar{'+'}), LikeType::symbol},
	//	{GCharDiap(gchar{'-'}, gchar{'-'}), LikeType::symbol},
	//	{GCharDiap(gchar{'*'}, gchar{'*'}), LikeType::symbol},
	//	{GCharDiap(gchar{'/'}, gchar{'/'}), LikeType::symbol},
	//	/*{')', Ops::endList},
	//	{'0', Ops::endList},
	//	{'1', Ops::endList},
	//	{'2', Ops::endList},
	//	{'3', Ops::endList},
	//	{'4', Ops::endList},
	//	{'5', Ops::endList},
	//	{'6', Ops::endList},
	//	{'7', Ops::endList},
	//	{'8', Ops::endList},
	//	{'9', Ops::endList}*/

	//}
};


enum class ReadState {
	none, startToken
};

class ParseContext : public CClass {
public:
	//PSexpr sexpr;
	size_t curPos;
	gstring curToken;
	ReadState state;
	LikeType likeType;
	ParseContext() {
		reset();
	}
	void reset() {
		curPos = 0;
		curToken = "";
		state = ReadState::none;
		likeType = LikeType::unknown;
	}
};

struct CharPredicats : public CClass {
	bool isDigit(gchar ch) {
		return ch >= '0' && ch <= '9';
	}
	bool isAlpha(gchar ch) {
		return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
	}
	bool isSpec(gchar ch) {
		return ch == '+' || ch == '-' || ch == '/' || ch == '*';
	}
	bool isWhitespace(gchar ch) {
		return ch == ' ' || ch == '\t' || ch == '/n';
	}
	bool isOpenParent(gchar ch) {
		return ch == '(';
	}
	bool isCloseParent(gchar ch) {
		return ch == ')';
	}
};
class Reader : public IReader, public CClass {
	IDIBuilder& diBuilder;
	CharPredicats preds;
	void parse(
		gstring& programText,
		function<void(const ParseContext& ctx)> fnTokenReaded,
		function<void()> fnStartList,
		function<void()> fnEndList
		) {
		ParseContext ctx;

		for (gchar ch : programText) {
			if (preds.isDigit(ch)) {
				if (ctx.state == ReadState::none) {
					ctx.state = ReadState::startToken;
					ctx.likeType = LikeType::number;
					ctx.curToken = gstring{ ch };
				}
				else if (ctx.state == ReadState::startToken) {
					ctx.curToken += gstring{ ch };
				}
			}
			else if (preds.isAlpha(ch) || preds.isSpec(ch)) {
				if (ctx.state == ReadState::none) {
					ctx.state = ReadState::startToken;
					ctx.likeType = LikeType::symbol;
					ctx.curToken = gstring{ ch };
				}
				else if (ctx.state == ReadState::startToken) {
					ctx.likeType = LikeType::symbol;
					ctx.curToken += gstring{ ch };
				}
			}
			else if (preds.isWhitespace(ch)) {
				if (ctx.state == ReadState::startToken) {
					fnTokenReaded(ctx);
					ctx.reset();
				}
			}
			else if (preds.isOpenParent(ch) || preds.isCloseParent(ch)) {
				if (ctx.state == ReadState::startToken) {
					fnTokenReaded(ctx);
				}
				ctx.reset();
				if (preds.isOpenParent(ch)) {
					fnStartList();
				}
				else if (preds.isCloseParent(ch)) {
					fnEndList();
				}
			}
		}
	}

	PSexpr parseNumber(gstring token, IProgram& program) {
		int32_t num = std::stoi(token);
		return diBuilder.createNumber(num);
	}
	PSexpr parseSymbol(gstring token, IProgram& program) {
		/*return diBuilder.createSymbol(token);*/
		return program.getProgramContext()->getScope()->get(token);
	}
public:
	Reader(IDIBuilder& diBuilder) : diBuilder{ diBuilder } {

	}
	
	virtual void read(gstring& programText, IProgram& program) {
		/*sReaderContext ctx;*/
		ReadTable table;

		PSexpr resSexpr;
		PSexpr curSexpr;
		bool isListStarted = false;
		vector<PSexpr> list{};
		
		auto handleToken = [this, &program, &curSexpr, &isListStarted, &list](const ParseContext& ctx) {
			PSexpr cur;
			if (ctx.likeType == LikeType::number) {
				cur = parseNumber(ctx.curToken, program);
			}
			else if (ctx.likeType == LikeType::symbol) {
				cur = parseSymbol(ctx.curToken, program);
			}
			if (isListStarted) {
				list.push_back(cur);
			}
			else {
				curSexpr = cur;
			}
		};
		auto handleStartList = [this, &curSexpr, &isListStarted]() {
			isListStarted = true;
		};
		auto handleEndList = [this, &list, &curSexpr, &isListStarted]() {
			isListStarted = false;
			curSexpr = diBuilder.createNil();
			if (list.size() > 0) {
				size_t cur = list.size();
				do {
					--cur;
					curSexpr = std::static_pointer_cast<Sexpr>(diBuilder.createCons(list[cur], curSexpr));
				} while (cur != 0);
				//for (size_t cur = list.size() - 1; cur != 0; --cur) {
				//	
				//}
			}

		};
		parse(programText, handleToken, handleStartList, handleEndList);
		program.addForm(curSexpr);

		//shared_ptr<IScope> scope = diBuilder.createScope();

		//shared_ptr<Number> num = diBuilder.createNumber(42);
		//shared_ptr<Number> num2 = diBuilder.createNumber(3);
		//gstring symName = "plus";
		//PSexpr symSexpr = program.getProgramContext()->getScope()->get(symName);
		///*PSexpr symSexpr = diBuilder.createSymbol(symName);*/

		//shared_ptr<Sexpr> nilSexpr = diBuilder.createNil();
		////auto nilSexpr = std::dynamic_pointer_cast<Sexpr>(nil);
		//auto numSexpr = std::dynamic_pointer_cast<Sexpr>(num);
		//auto num2Sexpr = std::dynamic_pointer_cast<Sexpr>(num2);
		//shared_ptr<Sexpr> sexpr = diBuilder.createCons(num2Sexpr, nilSexpr);
		//shared_ptr<Sexpr> sexpr2 = diBuilder.createCons(numSexpr, sexpr);
		//shared_ptr<Sexpr> sexpr3 = diBuilder.createCons(symSexpr, sexpr2);

		//program.addForm(sexpr3);
	}
};