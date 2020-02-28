#pragma once

#include <map>
#include <variant>
#include <memory>

//#include <boost/di.hpp>

#include "../share.h"
#include "../interfaces/IDIBuilder.h"
#include "../interfaces/DataStructs.h"
#include "../implements/DataStructs.h"

// test
#include "../test.h"

//namespace di = boost::di;
using std::map;
using std::shared_ptr;
using std::make_shared;

class DIBuilder : public IDIBuilder {
	//enum class EOpType {
	//	bind, create
	//};
	//map<Cid, 
public:

	DIBuilder() = default;
	//template<class Interface, class Implement, typename ...Args>
	//void bind() {
	//	bind_constructor(EOpType::bind, Args... args);
	//}

	//template<class Implement, typename ...Args>
	//shared_ptr<Implement> bind_constructor(EOpType opType, Args... args) {
	//	if (opType == EOpType::bind) {
	//		return shared_ptr<Implement>(nullptr);
	//	}
	//	return std::make_shared<Implement>(args...);
	//}

	template<class Interface>
	Interface& get() {

	}

	template<class Implement, typename ...Args>
	shared_ptr<Implement> create(Args... args) {
		/*return bind_constructor(EOpType::create, Args... args);*/
		return shared_ptr<Implement>{nullptr};
	}

	template<>
	shared_ptr<ITest> create<ITest>(int i, double d) {
		//ITest* ptr = new CTest(i, d);
		//return shared_ptr<ITest>(ptr);
		return make_shared<CTest>(i, d);
	}

	template<>
	shared_ptr<INumber> create<INumber>(int32_t num) {
		return shared_ptr<INumber>(new Number(num));
	}
};