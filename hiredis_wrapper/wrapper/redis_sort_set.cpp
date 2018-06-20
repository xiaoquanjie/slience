#include "redis_connection.hpp"
#include "redis_pool.hpp"
#include "convert.hpp"

template<typename T>
int RedisConnection::zadd(const char*key, float score, const T& member) {
	std::ostringstream oss;
	oss << member;
	return zadd(key, score, oss.str());
}

int RedisConnection::zadd(const char*key, float score, const std::string& member) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZADD %s %f %s", key, score, member.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int ret = 0;
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
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
int RedisConnection::zadds(const char*key, const T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "ZADD " + std::string(key) + " ";
	for (typename T::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
		std::ostringstream oss1;
		std::ostringstream oss2;
		oss1 << iter->first;
		oss2 << iter->second;
		k += oss2.str() + " " + oss1.str() + " ";
	}
	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int ret = 0;
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
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
void RedisConnection::zrange(const char*key, int beg_idx, int end_idx, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZRANGE %s %d %d", key, beg_idx, end_idx);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; ++idx) {
			ContainerAdd<T, typename T::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


template<typename T1, typename T2>
void RedisConnection::zrangewithscores(const char* key, int beg_idx, int end_idx, std::map<T1, T2>& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZRANGE %s %d %d withscores", key, beg_idx, end_idx);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; idx += 2) {
			std::istringstream iss1(std::string(reply->element[idx]->str, reply->element[idx]->len));
			std::istringstream iss2(std::string(reply->element[idx + 1]->str, reply->element[idx + 1]->len));
			T1 member;
			iss1 >> member;
			T2 score;
			iss2 >> score;
			values.insert(std::make_pair(member, score));
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T>
bool RedisConnection::zrem(const char*key, const T& member) {
	std::ostringstream oss;
	oss << member;
	return zrem(key, oss.str());
}

bool RedisConnection::zrem(const char*key, const std::string& member) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZREM %s %s", key, member.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	bool ret = false;
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
		ret = static_cast<bool>(reply->integer);
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
int  RedisConnection::zrem(const char*key, const T& members) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "ZREM " + std::string(key) + " ";
	for (typename T::const_iterator iter = members.begin(); iter != members.end(); ++iter) {
		std::ostringstream oss;
		oss << *iter;
		k += oss.str() + " ";
	}
	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int ret = false;
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
		ret = reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
void RedisConnection::zrevrange(const char*key, int beg_idx, int end_idx, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZREVRANGE %s %d %d", key, beg_idx, end_idx);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; ++idx) {
			ContainerAdd<T, typename T::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2>
void RedisConnection::zrevrangewithscores(const char* key, int beg_idx, int end_idx, std::map<T1, T2>& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZREVRANGE  %s %d %d withscores", key, beg_idx, end_idx);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; idx += 2) {
			std::istringstream iss1(std::string(reply->element[idx]->str, reply->element[idx]->len));
			std::istringstream iss2(std::string(reply->element[idx + 1]->str, reply->element[idx + 1]->len));
			T1 member;
			iss1 >> member;
			T2 score;
			iss2 >> score;
			values.insert(std::make_pair(member,score));
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

bool RedisConnection::zrank(const char* key, const char* member, int& rank) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZRANK %s %s",key, member);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	bool ret = false;
	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_NIL) {
			break;
		}
		rank = (int)reply->integer;
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::zrevrank(const char* key, const char* member, int& rank)
{
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZREVRANK %s %s", key, member);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	bool ret = false;
	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_NIL) {
			break;
		}
		rank = (int)reply->integer;
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

void RedisConnection::zcard(const char* key, int& count) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZCARD %s", key);
	if (!reply)
		M_CLOSE_CONNECTION(this);

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
		count = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::zcount(const char* key, double min, double max, int& count) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZCOUNT %s %ld %ld", key, min, max);
	if (!reply)
		M_CLOSE_CONNECTION(this);

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
		count = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::zincrby(const char* key, const char* member, double incr, double& score) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZINCRBY %s %d %s", key, incr, member);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		std::istringstream iss(std::string(reply->str, reply->len));
		iss >> score;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2, typename T3>
void RedisConnection::zrangebyscore(const char* key, T1 min, T2 max, T3& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::ostringstream oss1;
	oss1 << min;
	std::ostringstream oss2;
	oss2 << max;

	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZRANGEBYSCORE %s %s %s", key, oss1.str().c_str(),
		oss2.str().c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; ++idx) {
			ContainerAdd<T3, typename T3::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2, typename T3>
void RedisConnection::zrevrangebyscore(const char* key, T1 min, T2 max, T3& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::ostringstream oss1;
	oss1 << min;
	std::ostringstream oss2;
	oss2 << max;

	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZREVRANGEBYSCORE %s %s %s", key, oss1.str().c_str(),
		oss2.str().c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_ARRAY) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; ++idx) {
			ContainerAdd<T3, typename T3::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::zscore(const char* key, const char* member, double& score) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "ZSCORE  %s %s", key,member);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		std::istringstream iss(std::string(reply->str, reply->len));
		iss >> score;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}
