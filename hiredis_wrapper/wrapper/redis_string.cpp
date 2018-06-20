#include "redis_connection.hpp"
#include "redis_pool.hpp"

template<typename T>
void RedisConnection::set(const char* key, T value) {
	std::ostringstream oss;
	oss << value;
	set(key, oss.str());
}

void RedisConnection::set(const char* key, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SET %s %s", key, value.c_str());
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

void RedisConnection::set(const char* key, const char* value, unsigned int len) {
	set(key, std::string(value, len));
}

template<typename T>
bool RedisConnection::setnx(const char* key, T value) {
	std::ostringstream oss;
	oss << value;
	return setnx(key, oss.str());
}

bool RedisConnection::setnx(const char* key, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SETNX %s %s", key, value.c_str());
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	bool flag = false;
	do {
		if (reply->type == REDIS_REPLY_ERROR) {
			error = RedisException(reply->str);
			break;
		}
		if (reply->type != REDIS_REPLY_INTEGER) {
			error = RedisException(M_ERR_NOT_DEFINED);
			break;
		}
		flag = static_cast<bool>(reply->integer);
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return flag;
}

bool RedisConnection::setnx(const char* key, const char* value, unsigned int len) {
	return setnx(key, std::string(value, len));
}

template<typename T>
void RedisConnection::setex(const char* key, T value, time_t expire) {
	std::ostringstream oss;
	oss << value;
	setex(key, oss.str(), expire);
}

void RedisConnection::setex(const char* key, const std::string& value, time_t expire) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SETEX %s %d %s", key, expire, value.c_str());
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

void RedisConnection::setex(const char* key, const char* value, unsigned int len, time_t expire) {
	setex(key, std::string(value, len), expire);
}

template<typename T>
void RedisConnection::get(const char* key, T& value) {
	std::string v;
	get(key, v);
	std::istringstream iss(v);
	iss >> value;
}

void RedisConnection::get(const char* key, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "GET %s", key);
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

void RedisConnection::get(const char* key, char* value, unsigned int len)
{
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "GET %s", key);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	RedisException error;
	do
	{
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


void RedisConnection::incrby(const char* key, int step) {
	int value = 0;
	incrby(key, step, value);
}

template<typename T>
void RedisConnection::incrby(const char* key, int step, T& new_value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "INCRBY %s %d", key, step);
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
		new_value = (T)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}


template<typename T>
void RedisConnection::decrby(const char* key, int step, T& new_value) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "DECRBY " + std::string(key) + " %d";
	redisReply* reply = (redisReply*)w_redisCommand(*this, "DECRBY %s %d", key, step);
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
		new_value = (T)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

void RedisConnection::decrby(const char* key, int step) {
	int value = 0;
	decrby(key, step, value);
}


int RedisConnection::strlen(const char* key) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "STRLEN %s", key);
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

int RedisConnection::append(const char* key, const std::string& app_value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "APPEND %s %s", key, app_value.c_str());
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

int RedisConnection::append(const char* key, const char* value, unsigned int len) {
	return append(key, std::string(value, len));
}

int RedisConnection::setrange(const char* key, int beg_idx, const char* value, unsigned int len) {
	return setrange(key, beg_idx, std::string(value, len));
}

int RedisConnection::setrange(const char* key, int beg_idx, const std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SETRANGE %s %d %s", key, beg_idx, value.c_str());
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

void RedisConnection::getrange(const char* key, int beg_idx, int end_idx, std::string& value) {
	M_CHECK_REDIS_CONTEXT(_context);
	redisReply* reply = (redisReply*)w_redisCommand(*this, "GETRANGE %s %d %d", key, beg_idx, end_idx);
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
		value.clear();
		value.append(reply->str, reply->len);
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;
}

int RedisConnection::setbit(const char* key, unsigned int offset, int value) {
	M_CHECK_REDIS_CONTEXT(_context);
	if (value != 0)
		value = 1;
	std::string k = "SETBIT " + std::string(key) + " %d %d";
	redisReply* reply = (redisReply*)w_redisCommand(*this, "SETBIT %s %d %d", key, offset, value);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	value = 0;
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
		value = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return value;
}

int RedisConnection::getbit(const char* key, unsigned int offset) {
	M_CHECK_REDIS_CONTEXT(_context);
	std::string k = "GETBIT " + std::string(key) + " %d";
	redisReply* reply = (redisReply*)w_redisCommand(*this, "GETBIT %s %d", key, offset);
	if (!reply)
		M_CLOSE_CONNECTION(this);

	int value = 0;
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
		value = (int)reply->integer;
	} while (false);

	freeReplyObject(reply);
	if (!error.Empty())
		throw error;

	return value;
}
