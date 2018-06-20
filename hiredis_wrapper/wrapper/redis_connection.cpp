#include "redis_connection.hpp"

RedisConnection::RedisConnection(){
}

RedisConnection::RedisConnection(shard_ptr_t<_rediscontext_> context) {
	_context = context;
}

RedisConnection::RedisConnection(const RedisConnection& conn) {
	this->_context = conn._context;
}

RedisConnection::~RedisConnection(){
}

RedisConnection& RedisConnection::operator=(const RedisConnection& conn) {
	this->_context = conn._context;
	return *this;
}

unsigned long long RedisConnection::connectionid()const {
	unsigned long long v = (unsigned long long)(_context.get());
	return v;
}
