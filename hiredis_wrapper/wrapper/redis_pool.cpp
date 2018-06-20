#include "redis_pool.hpp"

RedisConnection RedisPool::GetConnection(const std::string& ip, unsigned short port) {
	return GetConnection(ip, port, 0);
}

RedisConnection RedisPool::GetConnection(const std::string& ip, unsigned short port, unsigned short database) {
	typedef _redis_detail::ThreadLocalData<_redisinfo_> RedisDataType;
	_redisinfo_* pinfo = (_redisinfo_*)&RedisDataType::data();
	unsigned long long unique_id = _redisaddr_::three_only_id((unsigned int)inet_addr(ip.c_str()),
		port, database);

	std::map<unsigned long long, shard_ptr_t<_rediscontext_> >::iterator iter = pinfo->_info.find(unique_id);
	if (iter != pinfo->_info.end()) {
		return RedisConnection(iter->second);
	}
	else {
		const struct timeval timeout = { 1, 500000 };
		redisContext* context = redisConnectWithTimeout(ip.c_str(), port, timeout);
		try {
			if (!context)
				throw RedisException(M_ERR_REDIS_CONNECT_FAIL);
			if (context->err != 0) {
				throw RedisException(context->errstr);
			}
			if (database != 0 && !_selectdb(context, database))
				throw RedisException(M_ERR_REDIS_SELECT_DB_ERROR);
		}
		catch (RedisException e) {
			_freeRedisContext(context);
			throw e;
		}

		shard_ptr_t<_rediscontext_> context_ptr = shard_ptr_t<_rediscontext_>(new _rediscontext_);
		context_ptr->_context = context;
		context_ptr->_ip = ip;
		context_ptr->_port = port;
		context_ptr->_db = database;
		pinfo->_info[unique_id] = context_ptr;
		pinfo->_revinfo[context_ptr] = unique_id;

		return RedisConnection(context_ptr);
	}
}

bool RedisPool::_selectdb(redisContext* context, unsigned short db) {
	if (!context)
		return false;

	redisReply* reply = (redisReply*)redisCommand(context, "select %d", db);
	if (!reply) {
		throw RedisException(context->errstr);
	}

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
	if (!error.Empty()) {
		throw error;
	}
	return true;
}

void RedisPool::_releaseConnection(shard_ptr_t<_rediscontext_> context) {
	do {
		typedef _redis_detail::ThreadLocalData<_redisinfo_> RedisDataType;
		_redisinfo_* pinfo = (_redisinfo_*)&RedisDataType::data();
		std::map<shard_ptr_t<_rediscontext_>, unsigned long long>::iterator iter = pinfo->_revinfo.find(context);
		if (iter == pinfo->_revinfo.end())
			break;

		pinfo->_info.erase(iter->second);
		pinfo->_revinfo.erase(iter);
	} while (false);
	_freeRedisContext(context->_context);
	context->_context = 0;
}

void RedisPool::ReleaseConnection(RedisConnection& con) {
	if (con._context) {
		_releaseConnection(con._context);
		con._context.reset();
	}
}

void RedisPool::_freeRedisContext(redisContext* context)
{
	redisFree(context);
}

void* w_redisCommand(RedisConnection& conn, const char *format, ...) {
	va_list ap;
	try{
		va_start(ap, format);
		redisReply* reply = (redisReply*)redisvCommand(conn._context->_context, format, ap);
		if (!reply) {
			// try again
			std::string ip = conn._context->_ip;
			unsigned short port = conn._context->_port;
			unsigned short database = conn._context->_db;
			RedisPool::ReleaseConnection(conn);
			conn = RedisPool::GetConnection(ip, port, database);
			reply = (redisReply*)redisvCommand(conn._context->_context, format, ap);
		}
		va_end(ap);
		return reply;
	}
	catch (RedisException e) {
		va_end(ap);
		throw e;
	}
	return 0;
}

#ifdef M_PLATFORM_WIN
long WinSockInit() {
	WSADATA wsa_data;
	long result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	return result;
}
static long gSocketInit = WinSockInit();
#endif