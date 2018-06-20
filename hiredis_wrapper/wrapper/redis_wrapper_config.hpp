#ifndef M_REDIS_WRAPPER_CONFIG_INCLUDE
#define M_REDIS_WRAPPER_CONFIG_INCLUDE

#include "../config.hpp"
#include "../hiredis.h"
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <assert.h>
#include <sstream>

#ifdef M_PLATFORM_WIN
#include <memory>
#else
#include <tr1/memory>
#include <pthread.h>
#include <arpa/inet.h>
#endif

#define shard_ptr_t std::tr1::shared_ptr

// redis异常
struct RedisException
{
	RedisException() :_code(-1) {}

	RedisException(const char* what):_code(-1) {
		_what.reset(new std::string(what));
	}

	RedisException(const std::string& what) :_code(-1) {
		_what.reset(new std::string(what));
	}

	std::string What()const {
		if (!_what)
			return std::string();
		return *_what;
	}

	int Code()const {
		return _code;
	}

	bool Empty()const {
		return (!_what);
	}

private:
	shard_ptr_t<std::string> _what;
	int _code;
};

template<typename T1,typename T2>
struct TriangleValule {
	bool _valid;
	T1   _key;
	T2   _value;

	std::string Key() {
		std::ostringstream oss;
		oss << this->_key;
		return oss.str();
	}
	void SetValue(const char* value, unsigned int len) {
		std::istringstream iss(std::string(value,len));
		iss >> this->_value;
	}
};

template<>
struct TriangleValule<std::string, std::string>{
	bool _valid;
	std::string   _key;
	std::string   _value;

	std::string Key() {
		return this->_key;
	}
	void SetValue(const char* value, unsigned int len) {
		this->_value.append(value, len);
	}
};

template<typename T>
struct TriangleValule<T, std::string> {
	bool _valid;
	T    _key;
	std::string  _value;

	std::string Key() {
		std::ostringstream oss;
		oss << this->_key;
		return oss.str();
	}
	void SetValue(const char* value, unsigned int len) {
		this->_value.append(value, len);
	}
};

template<typename T>
struct TriangleValule<std::string,T> {
	bool _valid;
	std::string  _key;
	T  _value;

	std::string Key() {
		return this->_key;
	}
	void SetValue(const char* value, unsigned int len) {
		std::istringstream iss(std::string(value, len));
		iss >> this->_value;
	}
};


// 线程局部对象
namespace _redis_detail {
#ifdef M_PLATFORM_WIN
	template<typename T>
	class ThreadLocalData
	{
	public:
		struct _init_{
			DWORD _tkey;
			_init_(){
				// 创建
				_tkey = TlsAlloc();
				assert(_tkey != TLS_OUT_OF_INDEXES);
			}
			~_init_(){
				TlsFree(_tkey);
			}
		};

		inline static T& data(){
			T* pv = 0;
			if (0 == (pv = (T*)TlsGetValue(_data._tkey))){
				pv = new T;
				TlsSetValue(_data._tkey, (void*)pv);
			}
			return *pv;
		}

	protected:
		ThreadLocalData(const ThreadLocalData&);
		ThreadLocalData& operator=(const ThreadLocalData&);

	private:
		static _init_ _data;
	};

#else
	template<typename T>
	class ThreadLocalData
	{
	public:
		struct _init_{
			pthread_key_t _tkey;
			_init_(){
				// 创建
				pthread_key_create(&_tkey, 0);
			}
			~_init_(){
				T* pv = (T*)pthread_getspecific(_tkey);
				pthread_key_delete(_tkey);
				delete pv;
			}
		};

		inline static T& data(){
			T* pv = 0;
			if (0 == (pv = (T*)pthread_getspecific(_data._tkey))){
				pv = new T;
				pthread_setspecific(_data._tkey, (void*)pv);
			}
			return *pv;
		}

	protected:
		ThreadLocalData(const ThreadLocalData&);
		ThreadLocalData& operator=(const ThreadLocalData&);

	private:
		static _init_ _data;
	};
#endif
	template<typename T>
	typename ThreadLocalData<T>::_init_ ThreadLocalData<T>::_data;
}

struct _rediscontext_ {
	redisContext*  _context;
	std::string    _ip;
	unsigned short _port;
	unsigned short _db;
};

// redis地址
struct _redisaddr_ {
	unsigned int _ip;
	unsigned short _port;
	unsigned short _db;

	unsigned long long two_only_id()const {
		unsigned long long v = ((unsigned long long)_ip << 32);
		v += ((unsigned long long)_port << 16);
		return v;
	}
	unsigned long long three_only_id()const {
		unsigned long long v = ((unsigned long long)_ip << 32);
		v += ((unsigned long long)_port << 16);
		v += _db;
		return v;
	}
	static unsigned long long two_only_id(unsigned int ip, unsigned short port){
		unsigned long long v = ((unsigned long long)ip << 32);
		v += ((unsigned long long)port << 16);
		return v;
	}
	static unsigned long long three_only_id(unsigned int ip, unsigned short port, unsigned short db){
		unsigned long long v = ((unsigned long long)ip << 32);
		v += ((unsigned long long)port << 16);
		v += db;
		return v;
	}
};

struct _redisinfo_ {
	std::map<unsigned long long, shard_ptr_t<_rediscontext_> > _info;
	std::map<shard_ptr_t<_rediscontext_>, unsigned long long> _revinfo;
};

struct _redisinfoset_ {
	std::set<_rediscontext_*> _contexts;

	~_redisinfoset_() {
		for (std::set<_rediscontext_*>::iterator iter = _contexts.begin();
			iter != _contexts.end(); ++iter) {
			redisFree((*iter)->_context);
			delete ((_rediscontext_*)*iter);
		}
		_contexts.clear();
	}
};


#define M_CHECK_REDIS_CONTEXT(context)\
if (!context || !context->_context) throw RedisException(M_ERR_REDIS_NOT_CONNECTED);

#define M_REDIS_CONTEXT(context) context->_context

#define M_CLOSE_CONNECTION(conn) {\
	RedisException e(conn->_context->_context->errstr); \
	RedisPool::ReleaseConnection(*conn); \
	throw e; }


class RedisConnection;
void* w_redisCommand(RedisConnection& conn, const char *format, ...);

///////////////////////////////////////////////////////////////////////////
#define M_ERR_CODE_NOT_DEFINED (-1)
///////////////////////////////////////////////////////////////////////////
#define M_ERR_NOT_DEFINED			("not defined error")
#define M_ERR_REDIS_CONNECT_FAIL	("redisConnect fail")
#define M_ERR_REDIS_NOT_CONNECTED	("redis is not connected")
#define M_ERR_REDIS_REPLY_NULL		("redis reply is null")
#define M_ERR_REDIS_TYPE_NOT_MATCH	("type doesn't match")
#define M_ERR_REDIS_KEY_NOT_EXIST	("key not exist")
#define M_ERR_REDIS_ARRAY_SIZE_NOT_MATCH	("array size doesn't match")
#define M_ERR_REDIS_SELECT_DB_ERROR ("select db error")

#endif