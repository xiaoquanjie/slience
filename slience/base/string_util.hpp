#ifndef M_BASE_STRING_UTIL_INCLUDE
#define M_BASE_STRING_UTIL_INCLUDE

#include "slience/base/config.hpp"
#include <string>

M_BASE_NAMESPACE_BEGIN

class StringUtil {
public:
	static std::string basename(const std::string& path) {
		char delimiters[] = { '\\', '/' };
		for (unsigned int idx = 0; idx < sizeof(delimiters) / sizeof(char); ++idx) {
			size_t pos = path.find_last_of(delimiters[idx]);
			if (pos != std::string::npos) {
				return path.substr(pos + 1, path.size() - pos);
			}
		}
		return path;
	}

	static std::string remove_from_end(const std::string& src, const std::string& tar) {
		size_t pos = src.rfind(tar);
		if (pos != std::string::npos) {
			return src.substr(0, pos);
		}
		return src;
	}

	static std::string directory(const std::string& path) {
		char delimiters[] = { '\\', '/' };
		for (unsigned int idx = 0; idx < sizeof(delimiters) / sizeof(char); ++idx) {
			size_t pos = path.find_last_of(delimiters[idx]);
			if (pos != std::string::npos) {
				return path.substr(0, pos);
			}
		}
		return path;
	}
};

M_BASE_NAMESPACE_END
#endif