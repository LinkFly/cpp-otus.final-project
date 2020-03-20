#pragma once

#include "share-base.h"
#include <fstream>

class System {
public:
	static bool readFile(gstring filename, gstring& out) {
		std::ifstream fin(filename, std::ios::in);
		if (!fin) {
			return false;
		}
		gstring res;
		for (gstring line; std::getline(fin, line);)
			res += (line + "\n");
		out = res;
		return true;
	}
};
