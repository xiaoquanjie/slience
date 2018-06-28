#ifndef M_BASE_TIMER_INCLUDE
#define M_BASE_TIMER_INCLUDE

#include "slience/base/config.hpp"
#include <time.h>
#include <string>

#ifndef M_PLATFORM_WIN
#include <sys/time.h>
#else
#include "slience/base/win.hpp"
#include <WinSock2.h>	// // for timeval 
#endif

M_BASE_NAMESPACE_BEGIN

class timer
{
public:
	timer();

	void restart();

	double elapsed() const;

	double elapsed_max() const;

	double elapsed_min() const;

private:
	clock_t _starttime;
};

class second_timer
{
public:
	second_timer();

	void restart();

	time_t elapsed() const;

private:
	time_t _starttime;
};

struct c_time {
	static tm* localtime(const time_t* t, tm* result);

	static tm* localtime(tm* result);

	static tm* gmtime(const time_t* t, tm* result);

	static tm* gmtime(tm* result);

	static time_t time(time_t* t);
};

class timestamp
{
public:
	timestamp();

	bool operator==(const timestamp& rhs)const;

	bool operator!=(const timestamp& rhs)const;

	bool operator<(const timestamp& rhs)const;

	bool operator<=(const timestamp& rhs)const;

	bool operator>(const timestamp& rhs)const;

	bool operator>=(const timestamp& rhs)const;

	long millisecond()const;

	long microsecond()const;

	time_t second()const;

	std::string format_ymd()const;

	void format_ymd(char* buf, size_t len)const;

	std::string format_ymd_hms()const;

	void format_ymd_hms(char* buf, size_t len)const;

	std::string format_ymd_hms_mil()const;

	void format_ymd_hms_mil(char* buf, size_t len)const;

	std::string format_mil()const;

	void format_mil(char* buf, size_t len)const;

private:
	timeval tv_;
};

M_BASE_NAMESPACE_END
#endif