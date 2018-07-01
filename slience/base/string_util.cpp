#include "slience/base/string_util.hpp"

M_BASE_NAMESPACE_BEGIN

std::string StringUtil::basename(const std::string& path) {
	char delimiters[] = { '\\', '/' };
	for (unsigned int idx = 0; idx < sizeof(delimiters) / sizeof(char); ++idx) {
		size_t pos = path.find_last_of(delimiters[idx]);
		if (pos != std::string::npos) {
			return path.substr(pos + 1, path.size() - pos);
		}
	}
	return path;
}

std::string StringUtil::remove_from_end(const std::string& src, const std::string& tar) {
	size_t pos = src.rfind(tar);
	if (pos != std::string::npos) {
		return src.substr(0, pos);
	}
	return src;
}

std::string StringUtil::directory(const std::string& path) {
	char delimiters[] = { '\\', '/' };
	for (unsigned int idx = 0; idx < sizeof(delimiters) / sizeof(char); ++idx) {
		size_t pos = path.find_last_of(delimiters[idx]);
		if (pos != std::string::npos) {
			return path.substr(0, pos);
		}
	}
	return path;
}

M_BASE_NAMESPACE_END