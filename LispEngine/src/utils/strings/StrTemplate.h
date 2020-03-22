#pragma once

#include "../../base/general-types.h"

#include <map>
#include <vector>

using std::map;
using std::vector;

class StrTemplate {
	const gstring leftSpec = "{{";
	gstring rightSpec = "}}";
	//gstring tmpl;
	vector<gstring> parts;
	map<size_t, gstring> varsPlaces;
public:
	StrTemplate(const gstring& tmpl) {// : tmpl{ tmpl } {
		size_t curPos;
		gstring curTmpl = tmpl;
		auto leftLen = leftSpec.size();
		auto rightLen = rightSpec.size();
		size_t curIdx = 0;
		while ((curPos = curTmpl.find(leftSpec)) != std::string::npos) {
			size_t nextPos = curTmpl.find(rightSpec, curPos + leftLen);
			if (nextPos == std::string::npos)
				break;
			gstring part = curTmpl.substr(0, curPos);
			gstring var = curTmpl.substr(curPos + leftLen, nextPos - (curPos + leftLen));
			parts.push_back(part);
			++curIdx;
			parts.push_back(var);
			varsPlaces[curIdx] = var;
			++curIdx;
			curTmpl = curTmpl.substr(nextPos + rightLen);
		}
		parts.push_back(curTmpl);
	}

	gstring str(map<gstring, gstring> vals) {
		gstring res;
		for (size_t i = 0; i < parts.size(); ++i) {
			gstring curPart;
			auto it = varsPlaces.find(i);
			if (it != varsPlaces.end()) {
				auto var = parts[i];
				curPart = vals[var];
			}
			else {
				curPart = parts[i];
			}
			res += curPart;
		}
		return res;
	}
};