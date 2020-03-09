#pragma once

#include "share-base.h"

enum class ETypeId { nil, pointerToAtom, pointerToCons, number, symbol, function, cons, custom };

class Global {
public:
	// meta handling ETypeId
	static const short_size typeIdBits = 4;
	/*shared_ptr<DIBuilder> diBuilder;*/
	shared_ptr<IMemoryManager> memoryManager;
	Global() {
		memoryManager = std::dynamic_pointer_cast<IMemoryManager>(make_shared<MemoryManager>());

	}
	/*shared_ptr<DIBuilder> getDiBuilder() {
		return make_shared<DIBuilder>();
	}*/

	IMemoryManager& getMemoryManager() const {
		return *memoryManager;
	}
};

const Global& getGlobal() {
	static Global global = Global{};
	return global;
}