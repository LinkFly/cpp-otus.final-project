#pragma once

#include <memory>

#include "../share.h"

using std::shared_ptr;

class IDIBuilder : public IInterface {
public:
	template<class Interface>
	/*virtual T& get() = 0;*/ //denied virtual
	shared_ptr<Interface>& get() {};

	template<class Interface, typename ...Args>
	/*virtual T create(Args... args) = 0;*/ //denied virtual
	shared_ptr<Interface> create(Args... args) { return shared_ptr<Interface>{nullptr}; }
};





