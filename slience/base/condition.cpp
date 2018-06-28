#include "slience/base/condition.hpp"
#include <assert.h>

M_BASE_NAMESPACE_BEGIN

#ifdef M_PLATFORM_WIN
Condition::Condition(MutexLock& mutex)
	:_mutex(mutex) {
	InitializeConditionVariable(&_cond);
}

Condition::~Condition() {
}

void Condition::wait() {
	SleepConditionVariableCS(&_cond, &(_mutex.mutex()), INFINITE);
}

bool Condition::wait(int millisecond) {
	DWORD milseds = millisecond; // second * 1000;
	BOOL  ret = SleepConditionVariableCS(&_cond, &(_mutex.mutex()),
		milseds);
	return (ret == 0
		&& GetLastError() == ERROR_TIMEOUT);
}

void Condition::notify() {
	WakeConditionVariable(&_cond);
}

void Condition::notifyall() {
	WakeAllConditionVariable(&_cond);
}
#else
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
Condition::Condition(MutexLock& mutex)
	:_mutex(mutex) {
	pthread_cond_init(&_cond, NULL);
}

Condition::~Condition() {
	pthread_cond_destroy(&_cond);
}

void Condition::wait() {
	pthread_cond_wait(&_cond, &(_mutex.mutex()));
}

bool Condition::wait(int millisecond) {
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timespec abstime;
	//clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec = now.tv_sec;
	abstime.tv_nsec = now.tv_usec * 1000 + millisecond * 1000 * 1000;
	return (ETIMEDOUT == pthread_cond_timedwait(&_cond, &(_mutex.mutex()), &abstime));
}

void Condition::notify() {
	pthread_cond_signal(&_cond);
}

void Condition::notifyall() {
	pthread_cond_broadcast(&_cond);
}
#endif

M_BASE_NAMESPACE_END