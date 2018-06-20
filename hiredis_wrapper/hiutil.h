#ifndef __HIUTIL_H_
#define __HIUTIL_H_

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "config.hpp"
#ifdef M_PLATFORM_WIN
#include "win32.h"
#else
#include <sys/types.h>
#endif

#include <stdarg.h>
#include <stdint.h>

#define HI_OK        0
#define HI_ERROR    -1
#define HI_EAGAIN   -2
#define HI_ENOMEM   -3

typedef int rstatus_t; /* return type */

#define LF                  (uint8_t) 10
#define CR                  (uint8_t) 13
#define CRLF                "\x0d\x0a"
#define CRLF_LEN            (sizeof("\x0d\x0a") - 1)

#define NELEMS(a)           ((sizeof(a)) / sizeof((a)[0]))

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))

#define SQUARE(d)           ((d) * (d))
#define VAR(s, s2, n)       (((n) < 2) ? 0.0 : ((s2) - SQUARE(s)/(n)) / ((n) - 1))
#define STDDEV(s, s2, n)    (((n) < 2) ? 0.0 : sqrt(VAR((s), (s2), (n))))

#define HI_INET4_ADDRSTRLEN (sizeof("255.255.255.255") - 1)
#define HI_INET6_ADDRSTRLEN \
    (sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255") - 1)
#define HI_INET_ADDRSTRLEN  MAX(HI_INET4_ADDRSTRLEN, HI_INET6_ADDRSTRLEN)
#define HI_UNIX_ADDRSTRLEN  \
    (sizeof(struct sockaddr_un) - offsetof(struct sockaddr_un, sun_path))



#define str3icmp(m, c0, c1, c2)                                                             \
    ((m[0] == c0 || m[0] == (c0 ^ 0x20)) &&                                                 \
     (m[1] == c1 || m[1] == (c1 ^ 0x20)) &&                                                 \
     (m[2] == c2 || m[2] == (c2 ^ 0x20)))

#define str4icmp(m, c0, c1, c2, c3)                                                         \
    (str3icmp(m, c0, c1, c2) && (m[3] == c3 || m[3] == (c3 ^ 0x20)))

#define str5icmp(m, c0, c1, c2, c3, c4)                                                     \
    (str4icmp(m, c0, c1, c2, c3) && (m[4] == c4 || m[4] == (c4 ^ 0x20)))

#define str6icmp(m, c0, c1, c2, c3, c4, c5)                                                 \
    (str5icmp(m, c0, c1, c2, c3, c4) && (m[5] == c5 || m[5] == (c5 ^ 0x20)))

#define str7icmp(m, c0, c1, c2, c3, c4, c5, c6)                                             \
    (str6icmp(m, c0, c1, c2, c3, c4, c5) &&                                                 \
     (m[6] == c6 || m[6] == (c6 ^ 0x20)))

#define str8icmp(m, c0, c1, c2, c3, c4, c5, c6, c7)                                         \
    (str7icmp(m, c0, c1, c2, c3, c4, c5, c6) &&                                             \
     (m[7] == c7 || m[7] == (c7 ^ 0x20)))

#define str9icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8)                                     \
    (str8icmp(m, c0, c1, c2, c3, c4, c5, c6, c7) &&                                         \
     (m[8] == c8 || m[8] == (c8 ^ 0x20)))

#define str10icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9)                                \
    (str9icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8) &&                                     \
     (m[9] == c9 || m[9] == (c9 ^ 0x20)))

#define str11icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10)                           \
    (str10icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9) &&                                \
     (m[10] == c10 || m[10] == (c10 ^ 0x20)))

#define str12icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11)                      \
    (str11icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10) &&                           \
     (m[11] == c11 || m[11] == (c11 ^ 0x20)))

#define str13icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12)                 \
    (str12icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11) &&                      \
     (m[12] == c12 || m[12] == (c12 ^ 0x20)))

#define str14icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13)            \
    (str13icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12) &&                 \
     (m[13] == c13 || m[13] == (c13 ^ 0x20)))

#define str15icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14)       \
    (str14icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13) &&            \
     (m[14] == c14 || m[14] == (c14 ^ 0x20)))

#define str16icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14, c15)  \
    (str15icmp(m, c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, c12, c13, c14) &&       \
     (m[15] == c15 || m[15] == (c15 ^ 0x20)))



/*
 * Wrapper to workaround well known, safe, implicit type conversion when
 * invoking system calls.
 */
#define hi_gethostname(_name, _len) \
    gethostname((char *)_name, (size_t)_len)

#define hi_atoi(_line, _n)          \
    _hi_atoi((uint8_t *)_line, (size_t)_n)
#define hi_itoa(_line, _n)          \
        _hi_itoa((uint8_t *)_line, (int)_n)

#define uint_len(_n)        \
    _uint_len((uint32_t)_n)


int hi_set_blocking(int sd);
int hi_set_nonblocking(int sd);
int hi_set_reuseaddr(int sd);

int _hi_atoi(uint8_t *line, size_t n);
void _hi_itoa(uint8_t *s, int num);

int hi_valid_port(int n);

int _uint_len(uint32_t num);


/*
 * Memory allocation and free wrappers.
 *
 * These wrappers enables us to loosely detect double free, dangling
 * pointer access and zero-byte alloc.
 */
#define hi_alloc(_s)                    \
    _hi_alloc((size_t)(_s), __FILE__, __LINE__)

#define hi_zalloc(_s)                   \
    _hi_zalloc((size_t)(_s), __FILE__, __LINE__)

#define hi_calloc(_n, _s)               \
    _hi_calloc((size_t)(_n), (size_t)(_s), __FILE__, __LINE__)

#define hi_realloc(_p, _s)              \
    _hi_realloc(_p, (size_t)(_s), __FILE__, __LINE__)

#define hi_free(_p) do {                \
    _hi_free(_p, __FILE__, __LINE__);   \
    (_p) = NULL;                        \
} while (0)

void *_hi_alloc(size_t size, const char *name, int line);
void *_hi_zalloc(size_t size, const char *name, int line);
void *_hi_calloc(size_t nmemb, size_t size, const char *name, int line);
void *_hi_realloc(void *ptr, size_t size, const char *name, int line);
void _hi_free(void *ptr, const char *name, int line);


#define ASSERT(_x) 
#define NOT_REACHED()


int _scnprintf(char *buf, size_t size, const char *fmt, ...);
int _vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
int64_t hi_usec_now(void);
int64_t hi_msec_now(void);
uint16_t crc16(const char *buf, int len);

#endif
