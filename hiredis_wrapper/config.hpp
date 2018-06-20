#ifndef M_HIREDIS_CONFIG_INCLUDE
#define M_HIREDIS_CONFIG_INCLUDE


#ifndef M_PLATFORM_WIN32
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define M_PLATFORM_WIN32 1
#endif
#endif

#ifndef M_PLATFORM_WIN
#if defined(M_PLATFORM_WIN32) || defined(WIN64)
#define M_PLATFORM_WIN 1
#endif
#endif


#ifdef M_PLATFORM_WIN

#define error_number WSAGetLastError()
#define set_error_number(err) WSASetLastError(err)

// socket error code
#ifndef M_ETIMEDOUT
#define M_ETIMEDOUT WSAETIMEDOUT
#endif

#ifndef M_EHOSTUNREACH
#define M_EHOSTUNREACH WSAEHOSTUNREACH 
#endif

#ifndef M_EINPROGRESS
#define M_EINPROGRESS WSAEWOULDBLOCK
#endif

#ifndef M_EADDRNOTAVAIL
#define M_EADDRNOTAVAIL WSAEADDRNOTAVAIL
#endif

#else
#define error_number errno
#define set_error_number(err) errno = err

// socket error code
#ifndef M_ETIMEDOUT
#define M_ETIMEDOUT ETIMEDOUT
#endif

#ifndef M_EHOSTUNREACH
#define M_EHOSTUNREACH EHOSTUNREACH 
#endif

#ifndef M_EINPROGRESS
#define M_EINPROGRESS EINPROGRESS
#endif

#ifndef M_EADDRNOTAVAIL
#define M_EADDRNOTAVAIL EADDRNOTAVAIL
#endif

#endif


#endif
