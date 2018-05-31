#ifndef M_BASE_SINGLETON_INCLUDE
#define M_BASE_SINGLETON_INCLUDE

#include "slience/base/config.hpp"
#include "slience/base/noncopyable.hpp"

M_BASE_NAMESPACE_BEGIN

template<typename T>
class singletion : public noncopyable {
public:
	static T& mutable_instance() {
		return instance();
	}

	static const T& const_instance() {
		return instance();
	}

private:
	static T & instance() {
		static T instance;
		return instance;
	}
};


M_BASE_NAMESPACE_END
#endif