#ifndef M_BASE_COMPATIBILITY_INCLUDE
#define M_BASE_COMPATIBILITY_INCLUDE

#include "slience/base/config.hpp"

#ifdef M_PLATFORM_WIN
// for windows
#include <stdio.h>

#if defined(_MSC_VER) && _MSC_VER<1900   // MS VC++ 14.0 _MSC_VER = 1900 (Visual Studio 2015)
#  define snprintf sprintf_s  
#endif 

#else
// for linux
#include <unistd.h>

#define Sleep(milsecond) usleep(milsecond * 1000)


#endif
#endif