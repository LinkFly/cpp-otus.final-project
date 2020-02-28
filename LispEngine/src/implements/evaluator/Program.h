#pragma once

#include <memory>

#include "../../share.h"
#include "../../interfaces/evaluator/IProgram.h"

class MemoryManager : public IMemoryManager, CClass {

}

class Program : public IProgram, CClass {
	unique_ptr<IMemoryManager> mman;


	constructor(unique_ptr<IMemoryManager> mman) override {

	}
public:
	Program(unique_ptr<IMemoryManager> mman) {
		constructor(mman);
	}
};