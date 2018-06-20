#ifndef M_REDIS_POOL_INCLUDE
#define M_REDIS_POOL_INCLUDE

#include "redis_connection.hpp"
#include "mutex.hpp"

// redis连接池
class RedisPool {
public:
	// 每个线程都会有一个连接,最好别跨线程使用,否则不保证线程安全
	static RedisConnection GetConnection(const std::string& ip, unsigned short port);

	// 每个线程都会有一个连接,最好别跨线程使用,否则不保证线程安全
	static RedisConnection GetConnection(const std::string& ip, unsigned short port, unsigned short database);

	// 释放连接
	static void ReleaseConnection(RedisConnection& con);

private:
	static void _releaseConnection(shard_ptr_t<_rediscontext_> context);

	static bool _selectdb(redisContext* context, unsigned short db);

	static void _freeRedisContext(redisContext* context);
};
#endif