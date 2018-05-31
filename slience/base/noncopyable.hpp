#ifndef M_BASE_NONCOPYABLE_INCLUDE
#define M_BASE_NONCOPYABLE_INCLUDE

#include "slience/base/config.hpp"

M_BASE_NAMESPACE_BEGIN

class noncopyable {
protected:
	noncopyable(const noncopyable&);
	noncopyable& operator=(const noncopyable&);
};

M_BASE_NAMESPACE_END
#endif