#include "redis_connection.hpp"
#include "redis_pool.hpp"
#include "convert.hpp"

template<typename T>
int RedisConnection::hset(const char* key, const char* field, const T& value) {
	std::ostringstream oss;
	oss << value;
	return hset(key, field, oss.str());
}

int RedisConnection::hset(const char* key, const char* field, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HSET %s %s %s", key, field, value.c_str());
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

int RedisConnection::hset(const char* key, const char* field, const char* value, unsigned int len) {
	return hset(key, field, std::string(value, len));
}


template<typename T>
void RedisConnection::hget(const char* key, const char* field, T& value) {
	std::string v;
	hget(key, field, v);
	std::istringstream iss(v);
	iss >> value;
}

void RedisConnection::hget(const char* key, const char* field, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HGET %s %s", key, field);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_NIL) {
			error = RedisException(M_ERR_REDIS_KEY_NOT_EXIST);
			break;
		}
		value.clear();
		value.append(reply->str, reply->len);
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::hget(const char* key, const char* field, char* value, unsigned int len) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HGET %s %s", key, field);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_NIL) {
			error = RedisException(M_ERR_REDIS_KEY_NOT_EXIST);
			break;
		}
		if (len > (unsigned int)reply->len)
			len = (unsigned int)reply->len;
		memcpy(value, reply->str, len);
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


template<typename T1, typename T2>
void RedisConnection::hgetall(const char* key, std::map<T1, T2>& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HGETALL %s", key);
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

			T1 value1;
			iss1 >> value1;
			T2 value2;
			iss2 >> value2;
			values.insert(std::make_pair(value1, value2));
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2, typename T3>
void RedisConnection::hgetall(const char* key, T1& values, std::pair<T2, T3>*) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HGETALL %s", key);
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

			T2 value1;
			iss1 >> value1;
			T3 value2;
			iss2 >> value2;
			values.push_back(std::make_pair(value1, value2));
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T>
bool RedisConnection::hsetnx(const char* key, const char* field, const T& value) {
	std::ostringstream oss;
	oss << value;
	return hsetnx(key, field, oss.str());
}

bool RedisConnection::hsetnx(const char* key, const char* field, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HSETNX %s %s %s", key, field, value.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	bool ret = 0;
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

bool RedisConnection::hsetnx(const char* key, const char* field, const char* value, unsigned int len) {
	return hsetnx(key, field, std::string(value, len));
}

template<typename T1, typename T2>
void RedisConnection::hmset(const char* key, const std::map<T1, T2>& values) {
	if (values.empty())
		return;

	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "HMSET " + std::string(key) + " ";
	for (typename std::map<T1, T2>::const_iterator iter = values.begin();
		iter != values.end(); ++iter) {
		std::ostringstream oss1;
		oss1 << iter->first;
		std::ostringstream oss2;
		oss2 << iter->second;
		k += oss1.str() + " " + oss2.str() + " ";
	}

	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_STATUS) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		if (strcasecmp(reply->str, "OK") != 0) {
			error = RedisException(reply->str);
			break;
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2, typename T3>
void RedisConnection::hmset(const char* key, const T1& values, std::pair<T2, T3>*)
{
	if (values.empty())
		return;

	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "HMSET " + std::string(key) + " ";
	for (typename T1::const_iterator iter = values.begin();
		iter != values.end(); ++iter) {
		std::ostringstream oss1;
		oss1 << iter->first;
		std::ostringstream oss2;
		oss2 << iter->second;
		k += oss1.str() + " " + oss2.str() + " ";
	}

	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_STATUS) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		if (strcasecmp(reply->str, "OK") != 0) {
			error = RedisException(reply->str);
			break;
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T1, typename T2>
void RedisConnection::hmget(const char* key, std::vector<TriangleValule<T1, T2> >&values) {
	if (values.empty())
		return;

	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "HMGET " + std::string(key) + " ";
	for (typename std::vector<TriangleValule<T1, T2> >::iterator iter = values.begin();
		iter != values.end(); ++iter) {
		k += iter->Key() + " ";
	}

	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
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
		if (reply->elements != values.size()) {
			error = RedisException(M_ERR_REDIS_ARRAY_SIZE_NOT_MATCH);
			break;
		}
		for (size_t idx = 0; idx < reply->elements; ++idx)
		{
			redisReply* rep = reply->element[idx];
			if (rep->type == REDIS_REPLY_NIL)
				values[idx]._valid = false;
			else {
				values[idx]._valid = true;
				values[idx].SetValue(rep->str, rep->len);
			}
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


template<typename T>
void RedisConnection::hincrby(const char* key, const char* field, int step, T& new_value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HINCRBY %s %s %d", key, field, step);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do
	{
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_INTEGER) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		new_value = (T)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::hincrby(const char* key, const char* field, int step) {
	int value = 0;
	hincrby(key, field, step, value);
}

bool RedisConnection::hexists(const char* key, const char*field) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HEXISTS %s %s", key, field);
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

int RedisConnection::hlen(const char* key) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HLEN %s", key);
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
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::hdel(const char* key, const char*field) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HDEL %s %s", key, field);
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
bool RedisConnection::hdel(const char*key, const T& field) {
	std::ostringstream oss;
	oss << field;
	return hdel(key, oss.str().c_str());
}

template<typename T>
void RedisConnection::hkeys(const char*key, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HKEYS %s", key);
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



template<typename T>
void RedisConnection::hvals(const char*key, T& values)
{
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = " ";
	redisReply* reply = (redisReply*)w_redisCommand(*this, "HVALS %s", key);
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

