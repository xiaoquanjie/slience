#include "hiutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>

#include "config.hpp"
#ifdef M_PLATFORM_WIN
#include "win32.h"
#else
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif

#ifdef HI_HAVE_BACKTRACE
# include <execinfo.h>
#endif

#ifndef M_PLATFORM_WIN

int hi_set_blocking(int sd)
{
    int flags;

    flags = fcntl(sd, F_GETFL, 0);
    if (flags < 0) {
        return flags;
    }

    return fcntl(sd, F_SETFL, flags & ~O_NONBLOCK);
}

int hi_set_nonblocking(int sd)
{
    int flags;

    flags = fcntl(sd, F_GETFL, 0);
    if (flags < 0) {
        return flags;
    }

    return fcntl(sd, F_SETFL, flags | O_NONBLOCK);
}

#else

int hi_set_blocking(int sd)
{
	unsigned long on = 0;
	return (ioctlsocket(sd, FIONBIO, &on) == 0);
}

int hi_set_nonblocking(int sd)
{
	unsigned long on = 1;
	return (ioctlsocket(sd, FIONBIO, &on) == 0);
}

#endif

int
_hi_atoi(uint8_t *line, size_t n)
{
	int value; 

    if (n == 0) {
        return -1;
    }

    for (value = 0; n--; line++) {
        if (*line < '0' || *line > '9') {
            return -1;
        }

        value = value * 10 + (*line - '0');
    }

    if (value < 0) {
        return -1;
    }

    return value;
}

void 
_hi_itoa(uint8_t *s, int num)
{
    uint8_t c;
    uint8_t sign = 0;

    if(s == NULL)
    {
        return;
    }

    uint32_t len, i;
    len = 0;

    if(num < 0)
    {
        sign = 1;
        num = abs(num);
    }
    else if(num == 0)
    {
        s[len++] = '0';
        return;
    }

    while(num % 10 || num /10)
    {
        c = num %10 + '0';
        num = num /10;
        s[len+1] = s[len];
        s[len] = c;
        len ++;
    }

    if(sign == 1)
    {
        s[len++] = '-';
    }

    s[len] = '\0';
    
    for(i = 0; i < len/2; i ++)
    {
        c = s[i];
        s[i] = s[len - i -1];
        s[len - i -1] = c;
    }

}


int
hi_valid_port(int n)
{
    if (n < 1 || n > UINT16_MAX) {
        return 0;
    }

    return 1;
}

int _uint_len(uint32_t num)
{
    int n = 0;

    if(num == 0)
    {
        return 1;
    }

    while(num != 0)
    {
        n ++;
        num /= 10;
    }

    return n;
}

void *
_hi_alloc(size_t size, const char *name, int line)
{
    void *p;

    ASSERT(size != 0);

    p = malloc(size);

    if(name == NULL && line == 1)
    {

    }

    return p;
}

void *
_hi_zalloc(size_t size, const char *name, int line)
{
    void *p;

    p = _hi_alloc(size, name, line);
    if (p != NULL) {
        memset(p, 0, size);
    }

    return p;
}

void *
_hi_calloc(size_t nmemb, size_t size, const char *name, int line)
{
    return _hi_zalloc(nmemb * size, name, line);
}

void *
_hi_realloc(void *ptr, size_t size, const char *name, int line)
{
    void *p;

    ASSERT(size != 0);

    p = realloc(ptr, size);

    if(name == NULL && line == 1)
    {

    }
    
    return p;
}

void
_hi_free(void *ptr, const char *name, int line)
{
    ASSERT(ptr != NULL);

    if(name == NULL && line == 1)
    {

    }

    free(ptr);
}


int
_vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    int n;

    n = vsnprintf(buf, size, fmt, args);

    /*
     * The return value is the number of characters which would be written
     * into buf not including the trailing '\0'. If size is == 0 the
     * function returns 0.
     *
     * On error, the function also returns 0. This is to allow idiom such
     * as len += _vscnprintf(...)
     *
     * See: http://lwn.net/Articles/69419/
     */
    if (n <= 0) {
        return 0;
    }

    if (n < (int) size) {
        return n;
    }

    return (int)(size - 1);
}

int
_scnprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = _vscnprintf(buf, size, fmt, args);
    va_end(args);

    return n;
}


/*
 * Return the current time in microseconds since Epoch
 */
int64_t
hi_usec_now(void)
{
#ifdef M_PLATFORM_WIN
#define EPOCHFILETIME   (116444736000000000UL)
	FILETIME ft;
	LARGE_INTEGER li;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	int64_t tt = (li.QuadPart - EPOCHFILETIME) / 10;
	return tt;
#else
	struct timeval now;
	int64_t usec;
	int status;

	status = gettimeofday(&now, NULL);
	if (status < 0) {
		return -1;
	}

	usec = (int64_t)now.tv_sec * 1000000LL + (int64_t)now.tv_usec;
	return usec;
#endif
}

/*
 * Return the current time in milliseconds since Epoch
 */
int64_t
hi_msec_now(void)
{
    return hi_usec_now() / 1000LL;
}


