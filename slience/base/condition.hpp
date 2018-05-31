#ifndef M_BASE_CONDITION_INCLUDE
#define M_BASE_CONDITION_INCLUDE

#include "slience/base/config.hpp"
#include "slience/base/mutexlock.hpp"
M_BASE_NAMESPACE_BEGIN

class  Condition {
public:
	Condition(MutexLock& mutex);

	~Condition();

	// 这个函数会自动解锁互斥量，等条件满足后又会自动锁上互斥量
	void wait();

	// returns true if time out, false otherwise.
	bool wait(int second);

	// 会唤配正在等待条件的线程
	void notify();

	void notifyall();

private:
	MutexLock& _mutex;
#ifdef M_PLATFORM_WIN
	CONDITION_VARIABLE _cond;
#else
	pthread_cond_t _cond;
#endif
};

#ifdef M_PLATFORM_WIN
inline Condition::Condition(MutexLock& mutex) 
	:_mutex(mutex){
	InitializeConditionVariable(&_cond);
}

inline Condition::~Condition() {
}

inline void Condition::wait() {
	SleepConditionVariableCS(&_cond, &(_mutex.mutex()), INFINITE);
}

inline bool Condition::wait(int millisecond) {
	DWORD milseds = millisecond; // second * 1000;
	BOOL  ret = SleepConditionVariableCS(&_cond, &(_mutex.mutex()), 
		milseds);
	return (ret == 0 
		&& GetLastError() == ERROR_TIMEOUT);
}

inline void Condition::notify() {
	WakeConditionVariable(&_cond);
}

inline void Condition::notifyall() {
	WakeAllConditionVariable(&_cond);
}
#else
#include <sys/time.h>
inline Condition::Condition(MutexLock& mutex)
	:_mutex(mutex) {
	pthread_cond_init(&_cond, NULL);
}

inline Condition::~Condition() {
	pthread_cond_destroy(&_cond);
}

inline void Condition::wait() {
	pthread_cond_wait(&_cond, &(_mutex.mutex()));
}

inline bool Condition::wait(int millisecond) {
	struct timeval now;
	gettimeofday(&now, NULL);
	struct timespec abstime;
	//clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec = now.tv_sec;
	abstime.tv_nsec = now.tv_usec * 1000 + millisecond * 1000 * 1000;
	return ETIMEDOUT == pthread_cond_timedwait(&_cond, &(_mutex.mutex()), &abstime);
}

inline void Condition::notify() {
	pthread_cond_signal(&_cond);
}

inline void Condition::notifyall() {
	pthread_cond_broadcast(&_cond);
}
#endif
M_BASE_NAMESPACE_END
#endif