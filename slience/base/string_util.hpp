#ifndef M_BASE_STRING_UTIL_INCLUDE
#define M_BASE_STRING_UTIL_INCLUDE

#include "slience/base/config.hpp"
#include <string>

M_BASE_NAMESPACE_BEGIN

class StringUtil {
public:
	static std::string basename(const std::string& path) {
#ifdef M_PLATFORM_WIN
		char delimiter = '\\';
#else
		char delimiter = '/';
#endif
		size_t pos = path.find_last_of(delimiter);
		if (pos != std::string::npos) {
			return path.substr(pos + 1, path.size() - pos);
		}
		return path;
	}

	static std::string remove_from_end(const std::string& src, const std::string& tar) {
		size_t pos = src.rfind(tar);
		printf("%ld\n", pos);
		if (pos != std::string::npos) {
			return src.substr(0, pos);
		}
		return src;
	}
};

M_BASE_NAMESPACE_END
#endif