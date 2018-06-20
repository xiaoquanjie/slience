#include "redis_connection.hpp"
#include "redis_pool.hpp"
#include "convert.hpp"

template<typename T>
int RedisConnection::sadd(const char*key, const T& value) {
	std::ostringstream oss;
	oss << value;
	return sadd(key, oss.str());
}

int RedisConnection::sadd(const char*key, const std::string&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SADD %s %s", key, value.c_str());
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
int RedisConnection::sadds(const char*key, const T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SADD " + std::string(key);
	for (typename T::const_iterator iter = values.begin();
		iter != values.end(); ++iter) {
		std::ostringstream oss;
		oss << *iter;
		k += " " + oss.str() + " ";
	}

	redisReply* reply = (redisReply*)w_redisCommand(*this, k.c_str());
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


// T要求是一个容器
template<typename T>
void RedisConnection::smembers(const char*key, T&values) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SMEMBERS %s", key);
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
		for (size_t idx = 0; idx < reply->elements; ++idx){
			ContainerAdd<T, typename T::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

template<typename T>
bool RedisConnection::spop(const char*key, T&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SPOP %s", key);
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
		std::istringstream iss(std::string(reply->str, reply->len));
		iss >> value;
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::spop(const char*key, std::string&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SPOP %s", key);
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

		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::spop(const char*key, char*value, unsigned int len) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SPOP %s", key);
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
bool RedisConnection::srem(const char*key, const T& field) {
	std::ostringstream oss;
	oss << field;
	return srem(key, oss.str().c_str());
}

bool RedisConnection::srem(const char*key, const char* field) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SREM %s %s", key, field);
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

int RedisConnection::scard(const char*key) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SCARD %s", key);
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

bool RedisConnection::sismember(const char*key, const char* field) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SISMEMBER %s %s",key,field);
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
bool RedisConnection::sismember(const char*key, const T& field) {
	std::ostringstream oss;
	oss << field;
	return sismember(key, oss.str().c_str());
}

template<typename T>
bool RedisConnection::smove(const char* src_key, const char* dst_key, const T& field) {
	std::ostringstream oss;
	oss << field;
	return smove(src_key, dst_key, oss.str().c_str());
}

bool RedisConnection::smove(const char* src_key, const char* dst_key, const char* field) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SMOVE %s %s %s", src_key, dst_key,field);
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
bool RedisConnection::srandmember(const char*key, T&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SRANDMEMBER %s", key);
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
		std::istringstream iss(std::string(reply->str, reply->len));
		iss >> value;
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

bool RedisConnection::srandmember(const char*key, std::string&value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SRANDMEMBER %s", key);
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
		value.clear();
		value.append(reply->str, reply->len);
		ret = true;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

template<typename T>
void RedisConnection::srandmember(const char*key, T&values, int count) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SRANDMEMBER %s %d", key,count);
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
void RedisConnection::sdiff(const char* key, const std::vector<std::string>& other_keys, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SDIFF " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
void RedisConnection::sunion(const char* key, const std::vector<std::string>& other_keys, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SUNION " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
void RedisConnection::sinter(const char* key, const std::vector<std::string>& other_keys, T& values) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SINTER " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
		}
		for (size_t idx = 0; idx < reply->elements; ++idx) {
			ContainerAdd<T, typename T::value_type>(values, reply->element[idx]->str, reply->element[idx]->len);
		}
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


int RedisConnection::sdiffstore(const char* key, const std::vector<std::string>& other_keys) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SDIFFSTORE " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
		}
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

int RedisConnection::sunionstore(const char* key, const std::vector<std::string>& other_keys) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SUNIONSTORE " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
		}
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}

int RedisConnection::sinterstore(const char* key, const std::vector<std::string>& other_keys) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "SINTERSTORE " + std::string(key) + " ";
	for (std::vector<std::string>::const_iterator iter = other_keys.begin(); iter != other_keys.end(); ++iter) {
		k += *iter + " ";
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
		}
		ret = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return ret;
}
