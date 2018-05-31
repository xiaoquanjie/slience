#ifndef M_BASE_GETOPT_INCLUDE
#define M_BASE_GETOPT_INCLUDE

#include <stdlib.h>
#include "slience/base/config.hpp"

#ifdef M_PLATFORM_WIN
#include "getopt.h"
#else
#include <getopt.h>
#endif


M_BASE_NAMESPACE_BEGIN

typedef struct option Option;

inline char* GetOptArg() {
	return optarg;
}

inline int GetOptArgI() {
	return atoi(optarg);
}

inline int GetOptLong(int ___argc, char *const *___argv,
	const char *__shortopts,
	const struct option *__longopts, int *__longind) {
	return getopt_long(___argc, ___argv, __shortopts, __longopts, __longind);
}

inline int GetOptLongOnly(int ___argc, char *const *___argv,
	const char *__shortopts,
	const struct option *__longopts, int *__longind) {
	return getopt_long_only(___argc, ___argv, __shortopts, __longopts, __longind);
}

M_BASE_NAMESPACE_END
#endif