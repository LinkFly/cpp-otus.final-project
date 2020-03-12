#pragma once

#include "share-base.h"

enum class ETypeId { nil, pointerToAtom, pointerToCons, number, symbol, function, cons, custom };

class Global {
public:
	// meta handling ETypeId
	static const short_size typeIdBits = 4;
	/*shared_ptr<DIBuilder> diBuilder;*/
	shared_ptr<IMemoryManager> memoryManager;
	shared_ptr<IRunContext> pRunContext;
	shared_ptr<IRunContext> pTopLevelRunContext;
	int* p;
	Global() {
		memoryManager = std::dynamic_pointer_cast<IMemoryManager>(make_shared<MemoryManager>());

	}
	/*shared_ptr<DIBuilder> getDiBuilder() {
		return make_shared<DIBuilder>();
	}*/

	IMemoryManager& getMemoryManager() const {
		return *memoryManager;
	}

	void setRunContext(shared_ptr<IRunContext>& ctx) {
		pRunContext = ctx;
	}
	shared_ptr<IRunContext>& getRunContext() {
		return this->pRunContext;
	}

	void setTopLevelRunContext(shared_ptr<IRunContext>& ctx) {
		pTopLevelRunContext = ctx;
	}
	shared_ptr<IRunContext>& getTopLevelRunContext() {
		return this->pTopLevelRunContext;
	}

	//DIBuilder& getDIBuilder() {
	//	return this->pTopLevelRunContext;
	//}

};

Global& getGlobal() {
	static Global global = Global{};
	return global;
}