#ifndef M_BASE_WIN_INCLUDE
#define M_BASE_WIN_INCLUDE

#ifdef M_PLATFORM_WIN
#ifndef M_WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN // 在所有Windows.h包含的地方加上它
#endif
#include <Windows.h>
#endif

#endif