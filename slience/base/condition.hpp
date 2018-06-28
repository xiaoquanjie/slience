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

M_BASE_NAMESPACE_END
#endif