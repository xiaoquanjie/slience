#include "slience/base/thread.hpp"

M_BASE_NAMESPACE_BEGIN

#ifndef M_PLATFORM_WIN
namespace detail {
	unsigned int tid() {
		static __thread unsigned int t_thread_id;
		if (__builtin_expect(t_thread_id == 0, 0)) {
			t_thread_id = gettid();
		}
		return t_thread_id;
	}

	const char* tid_str() {
		static __thread char t_thread_id_str[32];
		static __thread int t_id_str_flag;
		if (__builtin_expect(t_id_str_flag == 0, 0)) {
			t_id_str_flag = 1;
			snprintf(t_thread_id_str, sizeof(t_thread_id_str), "%5d ", tid());
		}
		return t_thread_id_str;
	}
}
#endif

/////////////////////////////////////////////////

thread::~thread() {
#ifdef M_PLATFORM_WIN
	CloseHandle(_handler);
#endif
}

void thread::join() {
	M_THREAD_JOIN(_handler);
}

void thread::detach() {
	M_CLOSE_THREAD_HANDLE(_handler);
}

unsigned int thread::tid()const {
	return _thrid;
}

const std::string thread::tid_str()const {
	return _thrid_str;
}

unsigned int thread::ctid() {
	return M_CUR_THREADID;
}

#ifdef M_PLATFORM_WIN
std::string thread::ctid_str() {
	unsigned int id = ctid();
	return std::to_string(id);
}
#else
const char* thread::ctid_str() {
	return detail::tid_str();
}
#endif

void thread::sleep(unsigned int milsec) {
	M_MIL_SLEEP(milsec);
}

M_BASE_NAMESPACE_END