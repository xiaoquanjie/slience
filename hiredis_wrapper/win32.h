#ifndef _WIN32_HELPER_INCLUDE
#define _WIN32_HELPER_INCLUDE

//#ifndef va_copy
//#define va_copy(d,s) ((d) = (s))
//#endif

#ifdef M_PLATFORM_WIN

#ifndef M_WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN // 
#endif

#pragma warning(disable: 4200)  //
#pragma warning(disable: 4244)	//
#pragma warning(disable: 4800)	//
#pragma warning(disable: 4996)	//

#include <Windows.h>
#include <WinSock2.h>
#include<ws2tcpip.h>
#pragma comment(lib,"WS2_32.lib")

#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define close closesocket
//#define write(p1, p2, p3) send(p1, p2, p3, 0) 

#if defined(_MSC_VER) && _MSC_VER<1900   // MS VC++ 14.0 _MSC_VER = 1900 (Visual Studio 2015)
#  define snprintf sprintf_s  
#endif 

// #define errno WSAGetLastError()

#endif // M_PLATFORM_WIN
#endif // _WIN32_HELPER_INCLUDE