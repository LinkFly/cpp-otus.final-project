#pragma once

#include "share-base.h"

#include <functional>

enum class ETypeId { nil, pointerToAtom, pointerToCons, number, symbol, function, lambda, cons, custom,
	string
};

class Global {
	
public:
	using IEvaluator = typename ::IEvaluator;
	// meta handling ETypeId
	static const short_size typeIdBits = 4;
	/*shared_ptr<DIBuilder> diBuilder;*/
	shared_ptr<IMemoryManager> memoryManager;
	shared_ptr<IRunContext> pRunContext;
	shared_ptr<IRunContext> pTopLevelRunContext;
	shared_ptr<IEvaluator> evaluator;
	IDIBuilder* diBuilder;

	Global() {
		memoryManager = std::dynamic_pointer_cast<IMemoryManager>(make_shared<MemoryManager>());

	}

	~Global() {
		if (memoryManager.get() != nullptr) {

		}
	}

	void setIDIBuilder(IDIBuilder* diBuilder) {
		this->diBuilder = diBuilder;
	}
	IDIBuilder*& getIDIBuilder() {
		return this->diBuilder;
	}
	//DIBuilder* getDiBuilder() {
	//	return reinterpret_cast<DIBuilder*>(this->diBuilder);
	//}
	/*shared_ptr<DIBuilder> getDiBuilder() {
		return make_shared<DIBuilder>();
	}*/

	IMemoryManager& getMemoryManager() const {
		return *memoryManager;
	}

	void setRunContext(shared_ptr<IRunContext>& ctx) {
		pRunContext = ctx;
	}
	shared_ptr<IRunContext> getRunContext() {
		if (this->pRunContext.get() != nullptr) {
			return this->pRunContext;
		}
		else {
			return this->pTopLevelRunContext;
		}
	}

	void setTopLevelRunContext(shared_ptr<IRunContext>& ctx) {
		pTopLevelRunContext = ctx;
	}
	shared_ptr<IRunContext>& getTopLevelRunContext() {
		return this->pTopLevelRunContext;
	}

	void setEvaluator(shared_ptr<IEvaluator> evaluator) {
		this->evaluator = evaluator;
	}

	shared_ptr<IEvaluator> getEvaluator() {
		return evaluator;
	}

	//DIBuilder& getDIBuilder() {
	//	return this->pTopLevelRunContext;
	//}
	

};

struct GlobalController {
	unique_ptr<Global> pGlobal;

	/*GlobalController() {
		pGlobal.swap(make_unique<Global>());
	}*/

	void setGlobal(unique_ptr<Global>& global) {
		pGlobal.swap(global);
	}
	unique_ptr<Global>& getGlobal() {
		return pGlobal;
	}

	void resetGlobal() {
		pGlobal.reset();
	}
};

GlobalController& getGlobalController() {
	static GlobalController globalController;
	return globalController;
}
//Global& getGlobal() {
//	return *getGlobalController().getGlobal();
//}

Global* global;

std::function<Global&()> fnGetGlobal;



void setFnGlobal(std::function<Global & ()>* func) {
	fnGetGlobal = *func;
}

void createGlobal() {
	global = new Global();
}

//Global& getGlobal() {
//	return *global;
//	//return fnGetGlobal();
//}

void setGlobal(Global* global_ptr) {
	global = global_ptr;
}

void freeGlobal() {
	delete global;
}




class LispEngineBase {
private:
	/*Global global;*/
public:
	virtual Global& getGlobal() = 0;/* {
		return global;
	}*/
};

LispEngineBase* lispEngine;

void setLispEngine(LispEngineBase* engine) {
	lispEngine = engine;
}

LispEngineBase* getLispEngine() {
	return lispEngine;
}

Global& getGlobal() {
	return lispEngine->getGlobal();
}

//struct GlobalGetter {
//	GlobalGetter(Global)
//};