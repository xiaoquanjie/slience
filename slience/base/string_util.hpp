#ifndef M_BASE_STRING_UTIL_INCLUDE
#define M_BASE_STRING_UTIL_INCLUDE

#include "slience/base/config.hpp"
#include <string>

M_BASE_NAMESPACE_BEGIN

class StringUtil {
public:
	static std::string basename(const std::string& path);

	static std::string remove_from_end(const std::string& src, const std::string& tar);

	static std::string directory(const std::string& path);
};

M_BASE_NAMESPACE_END
#endif