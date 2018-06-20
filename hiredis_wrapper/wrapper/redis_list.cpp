#include "redis_wrapper_config.hpp"
#include "redis_connection.hpp"
#include "redis_pool.hpp"
#include "convert.hpp"

template<typename T>
int RedisConnection::lpush(const char* key, const T& value) {
	std::ostringstream oss;
	oss << value;
	return lpush(key, oss.str());
}

int RedisConnection::lpush(const char* key, const std::string& value) {
	std::vector<std::string> vec;
	vec.push_back(value);
	return lpushs(key, vec);
}

template<typename T>
int RedisConnection::lpushs(const char* key, const T& values) {
	if (values.empty())
		return 0;

	//, typename T::value_type*
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "LPUSH " + std::string(key);
	for (typename T::const_iterator iter = values.begin();
		iter != values.end(); ++iter) {
		StringAdd<typename T::value_type>(k, *iter, " ");
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

template<typename T>
void RedisConnection::lrange(const char* key, int beg_idx, int end_idx, T& values, typename T::value_type*) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LRANGE %s %d %d", key, beg_idx, end_idx);
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
			redisReply* ele = reply->element[idx];
			std::istringstream iss(std::string(ele->str, ele->len));
			typename T::value_type v;
			iss >> v;
			values.push_back(v);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T>
void RedisConnection::lrange(const char* key, int beg_idx, int end_idx, T& values, std::string*) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LRANGE %s %d %d", key, beg_idx, end_idx);
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
			redisReply* ele = reply->element[idx];
			values.push_back(std::string(ele->str, ele->len));
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


template<typename T>
bool RedisConnection::lpop(const char* key, T& value) {
	std::string v;
	if (lpop(key, v)) {
		std::istringstream iss(v);
		iss >> value;
		return true;
	}
	return false;
}

bool RedisConnection::lpop(const char*key, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LPOP %s", key);
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
			ret = false;
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::lpop(const char*key, char* value, unsigned int len) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LPOP %s", key);
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
			ret = false;
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		if (len > (unsigned int)reply->len)
			len = (unsigned int)reply->len;
		memcpy(value, reply->str, len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
int RedisConnection::rpush(const char* key, const T& value) {
	std::vector<T> vec;
	vec.push_back(value);
	return rpushs(key, vec);
}

int RedisConnection::rpush(const char* key, const std::string& value) {
	std::vector<std::string> vec;
	vec.push_back(value);
	return rpushs(key, vec);
}

template<typename T>
int RedisConnection::rpushs(const char* key, const T& values) {
	if (values.empty())
		return 0;

	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "RPUSH " + std::string(key);
	for (typename T::const_iterator iter = values.begin();
		iter != values.end(); ++iter) {
		StringAdd<typename T::value_type>(k, *iter, " ");
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

template<typename T>
bool RedisConnection::rpop(const char* key, T& value) {
	std::string v;
	if (rpop(key, v)) {
		std::istringstream iss(v);
		iss >> value;
		return true;
	}
	return false;
}

bool RedisConnection::rpop(const char*key, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "RPOP %s", key);
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
			ret = false;
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::rpop(const char*key, char* value, unsigned int len) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "RPOP %s", key);
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
			ret = false;
			break;
		}
		if (reply->type != REDIS_REPLY_STRING) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		if (len > (unsigned int)reply->len)
			len = (unsigned int)reply->len;
		memcpy(value, reply->str, len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

int RedisConnection::llen(const char* key) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "LLEN " + std::string(key);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LLEN %s", key);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int len = 0;
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
		len = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return len;
}

template<typename T>
bool RedisConnection::lindex(const char* key, int idx, T&value) {
	std::string v;
	if (lindex(key, idx, v)) {
		std::istringstream iss(v);
		iss >> value;
		return true;
	}
	return false;
}

bool RedisConnection::lindex(const char* key, int idx, std::string&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LINDEX %s %d", key, idx);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	bool ret = false;
	RedisException error;
	do
	{
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type == REDIS_REPLY_NIL) {
			ret = false;
			break;
		}
		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::lindex(const char* key, int idx, char* value, unsigned int len) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LINDEX %s %d", key, idx);
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
			ret = false;
			break;
		}
		if (len > (unsigned int)reply->len)
			len = (unsigned int)reply->len;
		memcpy(value, reply->str, len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
void RedisConnection::lset(const char* key, int idx, T value) {
	std::ostringstream oss;
	oss << value;
	lset(key, idx, oss.str());
}

void RedisConnection::lset(const char* key, int idx, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LSET %s %d %s", key, idx, value.c_str());
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

void RedisConnection::lset(const char* key, int idx, const char* value, unsigned int len) {
	lset(key, idx, std::string(value, len));
}

template<typename T>
int RedisConnection::lrem(const char* key, int idx, T value) {
	std::ostringstream oss;
	oss << value;

	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LREM %s %d %s", key, idx, oss.str().c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int len = 0;
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
		len = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return len;
}

void RedisConnection::ltrim(const char*key, int beg_idx, int end_idx) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "LTRIM %s %d %d", key, beg_idx, end_idx);
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
int RedisConnection::linsert(const char* key, bool b_or_a, const T1& value1, const T2& value2) {
	std::ostringstream oss2;
	oss2 << value2;
	std::ostringstream oss1;
	oss1 << value1;
	return linsert(key, b_or_a, oss1.str(), oss2.str());
}

int RedisConnection::linsert(const char* key, bool b_or_a, const std::string& value1, const std::string&value2) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = std::string("LINSERT ") + std::string(key);
	if (b_or_a)
		k += std::string(" BEFORE ") + " %s %s";
	else
		k += std::string(" AFTER ") + " %s %s";
	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str(), value1.c_str(), value2.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int len = 0;
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
		len = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return len;
}

template<typename T>
bool RedisConnection::rpoplpush(const char*key1, const char*key2, T& value) {
	std::string v;
	if (rpoplpush(key1, key2, v)) {
		std::istringstream iss(v);
		v >> value;
		return true;
	}
	return false;
}

bool RedisConnection::rpoplpush(const char*key1, const char*key2, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "RPOPLPUSH %s %s", key1, key2);
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
			ret = false;
			break;
		}
		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}
