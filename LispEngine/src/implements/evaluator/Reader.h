#pragma once

#include <map>
#include <functional>
#include <vector>
#include <stack>

#include "../../share.h"
#include "../../interfaces/evaluator/IReader.h"
#include "../../interfaces/evaluator/IProgram.h"
//#include "../DiBuilder.h"
#include "../../interfaces/IDIBuilder.h"

using std::map;
using std::function;
using std::vector;
using std::stack;

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
		// TODO refactor: remove repeat - almost as isWhitespace
		if (ctx.state == ReadState::startToken) {
			fnTokenReaded(ctx);
		}
	}

	PSexpr parseNumber(gstring token, IProgram& program) {
		int32_t num = std::stoi(token);
		return diBuilder.createNumber(num);
	}
public:
	Reader(IDIBuilder& diBuilder) : diBuilder{ diBuilder } {

	}

	virtual PSexpr& parseSymbol(gstring token, IProgram& program) override {
		PSexpr& res = program.getProgramContext()->getScope()->get(token);
		if (res.get() != nullptr) {
			return res;
		}
		program.createSymbol(token);
		res = program.getProgramContext()->getScope()->get(token);
		return res;
	}
	
	virtual void read(gstring& programText, IProgram& program) {
		/*sReaderContext ctx;*/
		ReadTable table;
		PSexpr resSexpr;
		bool isListStarted = false;
		vector<PSexpr> list{};
		stack<PSexpr> listStack;
		stack<shared_ptr<vector<PSexpr>>> elemsStack;
		
		auto handleToken = [this, &listStack, &elemsStack, &program, &resSexpr, &isListStarted, &list](const ParseContext& ctx) {
			PSexpr cur;
			if (ctx.likeType == LikeType::number) {
				cur = parseNumber(ctx.curToken, program);
			}
			else if (ctx.likeType == LikeType::symbol) {
				cur = parseSymbol(ctx.curToken, program);
				if (cur.get() == nullptr) {
					cerr << "ERROR: Symbol `" << ctx.curToken << "` doesn't exists\n";
					exit(-1);
				}
			}
			if (!listStack.empty() && !elemsStack.empty()) {
				shared_ptr<vector<PSexpr>> curElemsList = elemsStack.top();
				curElemsList->push_back(cur);
			}
			else {
				resSexpr = cur;
			}
		};
		auto handleStartList = [this, &elemsStack, &listStack, &resSexpr, &isListStarted]() {
			isListStarted = true;
			auto pElemsList = make_shared<vector<PSexpr>>();
			elemsStack.push(pElemsList);
			auto nil = diBuilder.createNil();
			listStack.push(nil);
		};
		auto handleEndList = [this, &elemsStack, &listStack, &list, &resSexpr, &isListStarted]() {
			isListStarted = false;
			auto elems = elemsStack.top();
			elemsStack.pop();
			auto curSexpr = listStack.top();
			listStack.pop();
			
			if (elems->size() > 0) {
				size_t cur = elems->size();
				do {
					--cur;
					curSexpr = std::static_pointer_cast<Sexpr>(diBuilder.createCons((*elems)[cur], curSexpr));
				} while (cur != 0);
			}

			if (!listStack.empty() && !elemsStack.empty()) {
				shared_ptr<vector<PSexpr>> curElemsList = elemsStack.top();
				curElemsList->push_back(curSexpr);
			}
			else {
				resSexpr = curSexpr;
			}

		};
		parse(programText, handleToken, handleStartList, handleEndList);
		program.addForm(resSexpr);
	}
};