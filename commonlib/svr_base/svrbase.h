#ifndef M_SVR_BASE_INCLUDE
#define M_SVR_BASE_INCLUDE

#include "slience/base/buffer.hpp"
#include "slience/netio/netio.hpp"
#include "slience/base/timer.hpp"

#ifndef GETSETVAR
#define GETSETVAR(type, name) \
public: \
    const type& get_##name() const { return name##_; } \
    void set_##name(const type& newval) { name##_ = newval; } \
private: \
    type name##_;
#endif

#ifdef M_PLATFORM_WIN
#pragma pack(1)
struct AppHeadFrame {
#else
struct __attribute__((__packed__)) AppHeadFrame {
#endif
	GETSETVAR(base::s_uint32_t, cmd);
	GETSETVAR(base::s_uint16_t, is_broadcast);			// 消息是否广播
	GETSETVAR(base::s_uint32_t, src_svr_type);			// 源服务器类型
	GETSETVAR(base::s_uint32_t, dst_svr_type);			// 目标服务器类型
	GETSETVAR(base::s_uint32_t, src_instance_id);		// 源服务器实例
	GETSETVAR(base::s_uint32_t, dst_instance_id);		// 目标服务器实例
	GETSETVAR(base::s_uint32_t, src_transaction_id);	// 源事务id
	GETSETVAR(base::s_uint32_t, dst_transaction_id);	// 目标事务id
};
#ifdef M_PLATFORM_WIN
#pragma pack()
#endif

struct TcpSocketContext {
	netiolib::TcpSocketPtr ptr;
	int msgcount;
	base::timestamp tt;
};

struct TcpConnectorContext {
	netiolib::TcpConnector ptr;
	int msgcount;
	base::timestamp tt;
};

struct TcpSocketMsg {
	netiolib::TcpSocketPtr ptr;
	base::Buffer buf;
};

struct TcpConnectorMsg {
	netiolib::TcpConnectorPtr ptr;
	base::Buffer buf;
};

#endif