#include "slience/base/timer.hpp"
#include <limits>
#include <string>

M_BASE_NAMESPACE_BEGIN

timer::timer() {
	_starttime = clock();
}

void timer::restart() {
	_starttime = clock();
}

double timer::elapsed() const {
	return  double(clock() - _starttime) / CLOCKS_PER_SEC;
}

double timer::elapsed_max() const {
	return (double((std::numeric_limits<clock_t>::max)())
		- double(_starttime)) / double(CLOCKS_PER_SEC);
}

double timer::elapsed_min() const {
	return double(1) / double(CLOCKS_PER_SEC);
}

//////////////////////////////////////////////

second_timer::second_timer() {
	_starttime = time(0);
}

void second_timer::restart() {
	_starttime = time(0);
}

time_t second_timer::elapsed() const {
	return (time(0) - _starttime);
}

//////////////////////////////////////////////

tm* c_time::localtime(const time_t* t, tm* result) {
#ifndef M_PLATFORM_WIN
	result = ::localtime_r(t, result);
#else
	::localtime_s(result, t);
#endif
	return result;
}

tm* c_time::localtime(tm* result) {
	time_t t = c_time::time(0);
	return c_time::localtime(&t, result);
}

tm* c_time::gmtime(const time_t* t, tm* result) {
#ifndef M_PLATFORM_WIN
	result = ::gmtime_r(t, result);
#else
	::gmtime_s(result, t);
#endif
	return result;
}

tm* c_time::gmtime(tm* result) {
	time_t t = c_time::time(0);
	return c_time::gmtime(&t, result);
}

time_t c_time::time(time_t* t) {
	return ::time(t);
}

//////////////////////////////////////////////

timestamp::timestamp() {
#ifndef M_PLATFORM_WIN
	gettimeofday(&tv_, NULL);
#else
	SYSTEMTIME st;
	struct tm  tm;
	time_t     clock;

	GetLocalTime(&st);
	tm.tm_year = st.wYear - 1900;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_mday = st.wDay;
	tm.tm_hour = st.wHour;
	tm.tm_min = st.wMinute;
	tm.tm_sec = st.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv_.tv_sec = (long)clock;
	tv_.tv_usec = st.wMilliseconds * 1000;
#endif
}

bool timestamp::operator==(const timestamp& rhs)const {
	return (rhs.tv_.tv_sec == this->tv_.tv_sec && rhs.tv_.tv_usec == this->tv_.tv_usec);
}

bool timestamp::operator!=(const timestamp& rhs)const {
	return !(rhs == *this);
}

bool timestamp::operator<(const timestamp& rhs)const {
	return (this->tv_.tv_sec * 1000000 + this->tv_.tv_usec)<(rhs.tv_.tv_sec * 1000000 + rhs.tv_.tv_usec);
}

bool timestamp::operator<=(const timestamp& rhs)const {
	return !(*this>rhs);
}

bool timestamp::operator>(const timestamp& rhs)const {
	return (this->tv_.tv_sec * 1000000 + this->tv_.tv_usec)>(rhs.tv_.tv_sec * 1000000 + rhs.tv_.tv_usec);
}

bool timestamp::operator>=(const timestamp& rhs)const {
	return !(*this<rhs);
}

long timestamp::millisecond()const {
	return (this->tv_.tv_sec * 1000 + this->tv_.tv_usec / 1000);
}

long timestamp::microsecond()const {
	return (this->tv_.tv_sec * 1000000 + this->tv_.tv_usec);
}

time_t timestamp::second()const {
	return (tv_.tv_sec + tv_.tv_usec / (1000 * 1000));
}
std::string timestamp::format_ymd()const {
	char carray[20] = { 0 };
	format_ymd(carray, 20);
	return std::string(carray);
}

void timestamp::format_ymd(char* buf, size_t len)const {
	struct tm tm;
	time_t t = second();
	c_time::localtime(&t, &tm);
	snprintf(buf, len, "%4d%02d%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

std::string timestamp::format_ymd_hms()const {
	char carray[30] = { 0 };
	format_ymd_hms(carray, 30);
	return std::string(carray);
}

void timestamp::format_ymd_hms(char* buf, size_t len)const {
	struct tm tm;
	time_t t = second();
	c_time::localtime(&t, &tm);
	snprintf(buf, len, "%4d%02d%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec);
}

std::string timestamp::format_ymd_hms_mil()const {
	char carray[40] = { 0 };
	format_ymd_hms_mil(carray, 40);
	return std::string(carray);
}

void timestamp::format_ymd_hms_mil(char* buf, size_t len)const {
	struct tm tm;
	time_t t = second();
	c_time::localtime(&t, &tm);
	snprintf(buf, len, "%4d%02d%02d %02d:%02d:%02d.%03ld", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec, millisecond());
}

std::string timestamp::format_mil()const {
	char carray[10] = { 0 };
	format_mil(carray, 10);
	return std::string(carray);
}

void timestamp::format_mil(char* buf, size_t len)const {
	snprintf(buf, len, "%03ld", millisecond());
}


M_BASE_NAMESPACE_END