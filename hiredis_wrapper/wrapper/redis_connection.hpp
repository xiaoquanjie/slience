#ifndef M_REDIS_CONNECTION_INCLUDE
#define M_REDIS_CONNECTION_INCLUDE

#include "redis_wrapper_config.hpp"

//http://www.cnblogs.com/hjwublog/p/5639990.html
//https://redis.io/commands/setbit


// redis连接
class RedisConnection {
public:
	shard_ptr_t<_rediscontext_> _context;

public:
	RedisConnection();

	RedisConnection(shard_ptr_t<_rediscontext_> context);

	RedisConnection(const RedisConnection& conn);

	~RedisConnection();

	RedisConnection& operator=(const RedisConnection& conn);

	bool connected()const {
		return (_context != 0);
	}

	unsigned long long connectionid()const;

	////////////////////////////////////////////////////////////////////

	// 超时命令
	bool expire(const char* key, time_t expire);

	// 1为key存在，0表示key不存在
	int del(const char* key);

	template<typename T>
	int dels(const T& keys);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void set(const char* key, T value);

	void set(const char* key, const std::string& value);

	void set(const char* key, const char* value, unsigned int len);
	
	// 设置key对应的值为String类型的value，如果key已经存在则返回false
	template<typename T>
	bool setnx(const char* key, T value);
	
	bool setnx(const char* key, const std::string& value);
	
	bool setnx(const char* key, const char* value, unsigned int len);

	// 设置值，并设置一个超时 
	template<typename T>
	void setex(const char* key, T value, time_t expire);
	
	void setex(const char* key, const std::string& value, time_t expire);
	
	void setex(const char* key, const char* value, unsigned int len, time_t expire);
	
	////////////////////////////////////////////////////////////////////

	template<typename T>
	void get(const char* key, T& value);

	void get(const char* key, std::string& value);
	
	void get(const char* key, char* value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	void incrby(const char* key, int step);

	template<typename T>
	void incrby(const char* key, int step, T& new_value);

	template<typename T>
	void decrby(const char* key, int step, T& new_value);
	
	void decrby(const char* key, int step);

	////////////////////////////////////////////////////////////////////

	// 返回长度
	int strlen(const char* key);

	// 返回新长度
	int append(const char* key, const std::string& app_value);
	
	int append(const char* key, const char* value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	// 返回修改后字符串长度
	int setrange(const char* key, int beg_idx, const char* value, unsigned int len);
	
	int setrange(const char* key, int beg_idx, const std::string& value);
	
	void getrange(const char* key, int beg_idx, int end_idx, std::string& value);

	////////////////////////////////////////////////////////////////////

	// 返回原位的值，只会是0和1
	int setbit(const char* key, unsigned int offset, int value);
	
	int getbit(const char* key, unsigned int offset);

	////////////////////////////////////////////////////////////////////

	// 返回push的条数
	template<typename T>
	int lpush(const char* key, const T& value);

	int lpush(const char* key, const std::string& value);
	
	template<typename T>
	int lpushs(const char* key, const T& values);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void lrange(const char* key, int beg_idx, int end_idx, T& values, typename T::value_type*);
	
	template<typename T>
	void lrange(const char* key, int beg_idx, int end_idx, T& values, std::string*);

	////////////////////////////////////////////////////////////////////

	// 当元素不存在时返回False
	template<typename T>
	bool lpop(const char* key, T& value);
	
	bool lpop(const char*key, std::string& value);
	
	bool lpop(const char*key, char* value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	int rpush(const char* key, const T& value);
	
	int rpush(const char* key, const std::string& value);
	
	template<typename T>
	int rpushs(const char* key, const T& values);
	
	////////////////////////////////////////////////////////////////////

	// 当元素不存在时返回False
	template<typename T>
	bool rpop(const char* key, T& value);
	
	bool rpop(const char*key, std::string& value);
	
	bool rpop(const char*key, char* value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	int llen(const char* key);

	// 如果元素不存在返回false
	template<typename T>
	bool lindex(const char* key, int idx, T&value);
	
	bool lindex(const char* key, int idx, std::string&value);
	
	bool lindex(const char* key, int idx, char* value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	/*
	   将列表 key 下标为 index 的元素的值设置为 value.
	   当 index 参数超出范围，或对一个空列表( key 不存在)进行 LSET 时，返回一个错误.
	 */
	template<typename T>
	void lset(const char* key, int idx, T value);
	
	void lset(const char* key, int idx, const std::string& value);
	
	void lset(const char* key, int idx, const char* value, unsigned int len);
	
	template<int N>
	void lset(const char* key, int idx, const char(&value)[N]);

	////////////////////////////////////////////////////////////////////

	/*
		根据参数 count 的值，移除列表中与参数 value 相等的元素.
		count 的值可以是以下几种：
		count > 0 : 从表头开始向表尾搜索，移除与 value 相等的元素，数量为 count 。
		count < 0 : 从表尾开始向表头搜索，移除与 value 相等的元素，数量为 count 的绝对值。
		count = 0 : 移除表中所有与 value 相等的值。
	*/
	template<typename T>
	int lrem(const char* key, int idx, T value);

	/*
		对一个列表进行修剪(trim)，就是说，让列表只保留指定区间内的元素，不在指定区间之内的元素都将被删除。
	*/
	void ltrim(const char*key, int beg_idx, int end_idx);

	// 返回列表的长度
	template<typename T1, typename T2>
	int linsert(const char* key, bool b_or_a, const T1& value1, const T2& value2);
	
	int linsert(const char* key, bool b_or_a, const std::string& value1, const std::string&value2);

	/*
		一个原子时间内，执行以下两个动作:
		将列表 source 中的最后一个元素(尾元素)弹出，并返回给客户端。
		将 source 弹出的元素插入到列表 destination ，作为 destination 列表的的头元素。
	*/
	template<typename T>
	bool rpoplpush(const char*key1, const char*key2, T& value);
	
	bool rpoplpush(const char*key1, const char*key2, std::string& value);

	////////////////////////////////////////////////////////////////////

	// 返回1表示新设，返回0表示更新
	template<typename T>
	int hset(const char* key, const char* field, const T& value);
	
	int hset(const char* key, const char* field, const std::string& value);
	
	int hset(const char* key, const char* field, const char* value, unsigned int len);
	
	template<typename T>
	void hget(const char* key, const char* field, T& value);

	void hget(const char* key, const char* field, std::string& value);
	
	void hget(const char* key, const char* field, char* value, unsigned int len);

	template<typename T1, typename T2>
	void hgetall(const char* key, std::map<T1, T2>& values);
	
	template<typename T1, typename T2, typename T3>
	void hgetall(const char* key, T1& values, std::pair<T2, T3>*);

	////////////////////////////////////////////////////////////////////

	// 如果key已存在，则设置失败
	template<typename T>
	bool hsetnx(const char* key, const char* field, const T& value);
	
	bool hsetnx(const char* key, const char* field, const std::string& value);
	
	bool hsetnx(const char* key, const char* field, const char* value, unsigned int len);
	
	template<int N>
	bool hsetnx(const char* key, const char* field, const char(&value)[N]);

	////////////////////////////////////////////////////////////////////

	template<typename T1, typename T2>
	void hmset(const char* key, const std::map<T1, T2>& values);

	template<typename T1, typename T2, typename T3>
	void hmset(const char* key, const T1& values, std::pair<T2, T3>*);

	// values是in-out值
	template<typename T1, typename T2>
	void hmget(const char* key, std::vector<TriangleValule<T1, T2> >&values);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void hincrby(const char* key, const char* field, int step, T& new_value);
	
	void hincrby(const char* key, const char* field, int step);

	bool hexists(const char* key, const char*field);
	
	int  hlen(const char* key);
	
	bool hdel(const char* key, const char*field);
	
	template<typename T>
	bool hdel(const char*key, const T& field);

	// T要求是一个容器
	template<typename T>
	void hkeys(const char*key, T& values);

	// T要求是一个容器
	template<typename T>
	void hvals(const char*key, T& values);

	////////////////////////////////////////////////////////////////////

	// 返回成功插入的个数
	template<typename T>
	int sadd(const char*key, const T& value);
	
	int sadd(const char*key, const std::string&value);
	
	template<typename T>
	int sadds(const char*key, const T& values);

	////////////////////////////////////////////////////////////////////

	// T要求是一个容器,返回集合 key 中的所有成员
	template<typename T>
	void smembers(const char*key, T&values);
	
	template<typename T>
	bool spop(const char*key, T&value);
	
	bool spop(const char*key, std::string&value);
	
	bool spop(const char*key, char*value, unsigned int len);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	bool srem(const char*key, const T& field);

	bool srem(const char*key, const char* field);

	////////////////////////////////////////////////////////////////////

	int scard(const char*key);

	bool sismember(const char*key, const char* field);
	
	template<typename T>
	bool sismember(const char*key, const T& field);
	
	template<typename T>
	bool smove(const char* src_key, const char* dst_key, const T& field);
	
	bool smove(const char* src_key, const char* dst_key, const char* field);

	////////////////////////////////////////////////////////////////////

	// 随机返回一个元素
	template<typename T>
	bool srandmember(const char*key, T&value);
	
	bool srandmember(const char*key, std::string&value);
	
	// T要求是一个容器，随机返回count个元素
	template<typename T>
	void srandmember(const char*key, T&values, int count);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void sdiff(const char* key, const std::vector<std::string>& other_keys, T& values);
	
	template<typename T>
	void sunion(const char* key, const std::vector<std::string>& other_keys, T& values);

	template<typename T>
	void sinter(const char* key, const std::vector<std::string>& other_keys, T& values);
	
	/*
		这个命令的作用和 SDIFF 类似，但它将结果保存到 destination 集合，而不是简单地返回结果集。
		如果 destination 集合已经存在，则将其覆盖。
		destination 可以是 key 本身。
	*/
	int sdiffstore(const char* key, const std::vector<std::string>& other_keys);
	
	int sunionstore(const char* key, const std::vector<std::string>& other_keys);
	
	int sinterstore(const char* key, const std::vector<std::string>& other_keys);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	int zadd(const char*key, float score, const T& member);

	int zadd(const char*key, float score, const std::string& member);

	// T要求是容器，且元素是pair结构
	template<typename T>
	int zadds(const char*key, const T& values);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void zrange(const char*key, int beg_idx, int end_idx, T& values);
	
	template<typename T1, typename T2>
	void zrangewithscores(const char* key, int beg_idx, int end_idx,
		std::map<T1, T2>& values);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	bool zrem(const char*key, const T& member);
	
	bool zrem(const char*key, const std::string& member);
	
	template<typename T>
	int  zrem(const char*key, const T& members);

	////////////////////////////////////////////////////////////////////

	template<typename T>
	void zrevrange(const char*key, int beg_idx, int end_idx, T& values);
	
	template<typename T1, typename T2>
	void zrevrangewithscores(const char* key, int beg_idx, int end_idx, std::map<T1, T2>& values);

	////////////////////////////////////////////////////////////////////

	bool zrank(const char* key, const char* member, int& rank);
	
	bool zrevrank(const char* key, const char* member, int& rank);

	////////////////////////////////////////////////////////////////////

	void zcard(const char* key, int& count);
	
	// min 到 max之间的个数
	void zcount(const char* key, double min, double max, int& count);

	void zincrby(const char* key, const char* member, double incr, double& score);

	////////////////////////////////////////////////////////////////////

	// 返回有序集 key 中，所有 score 值介于 min 和 max 之间(包括等于 min 或 max )的成员。有序集成员按 score 值递增(从小到大)次序排列
	template<typename T1, typename T2, typename T3>
	void zrangebyscore(const char* key, T1 min, T2 max, T3& values);
	
	template<typename T1, typename T2, typename T3>
	void zrevrangebyscore(const char* key, T1 min, T2 max, T3& values);
	
	////////////////////////////////////////////////////////////////////

	void zscore(const char* key, const char* member, double& score);
};



#endif