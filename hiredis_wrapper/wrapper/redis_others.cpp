#ifndef M_REDIS_OTHERS_INCLUDE
#define M_REDIS_OTHERS_INCLUDE

#include "redis_connection.hpp"
#include "redis_pool.hpp"


bool RedisConnection::expire(const char* key, time_t expire)
{
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "EXPIRE " + std::string(key) + " %d";
	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str(), expire);
	if (!reply)
		M_CLOSE_CONNECTION(this);
	
	int success_flag = 0;
	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_INTEGER) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		success_flag = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return (success_flag == 1 ? true : false);
}

int RedisConnection::del(const char* key) {
	std::vector<std::string> vec;
	vec.push_back(key);
	return dels(vec);
}

template<typename T>
int RedisConnection::dels(const T& keys) {
	if (keys.empty())
		return 0;

	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "DEL ";
	for (typename T::const_iterator iter = keys.begin();
		iter != keys.end(); ++iter) {
		k += *iter + " ";
	}

	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int size = 0;
	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_INTEGER) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		size = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return size;
}

#endif