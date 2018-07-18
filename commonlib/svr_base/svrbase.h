#ifndef M_SVR_BASE_INCLUDE
#define M_SVR_BASE_INCLUDE

#include "slience/base/buffer.hpp"
#include "slience/netio/netio.hpp"
#include "slience/base/timer.hpp"
#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/ordered_index.hpp"

#ifndef GETSETVAR
#define GETSETVAR(type, name) \
public: \
    const type& get_##name() const { return name; } \
    void set_##name(const type& newval) { name = newval; } \
private: \
    type name;
#endif

#ifdef M_PLATFORM_WIN
#pragma pack(1)
struct AppHeadFrame {
#else
struct __attribute__((__packed__)) AppHeadFrame {
#endif
	GETSETVAR(base::s_uint16_t, is_broadcast);			// 消息是否广播
	GETSETVAR(base::s_uint32_t, src_svr_type);			// 源服务器类型
	GETSETVAR(base::s_uint32_t, dst_svr_type);			// 目标服务器类型
	GETSETVAR(base::s_uint32_t, src_instance_id);		// 源服务器实例
	GETSETVAR(base::s_uint32_t, dst_instance_id);		// 目标服务器实例
	GETSETVAR(base::s_uint32_t, src_transaction_id);	// 源事务id
	GETSETVAR(base::s_uint32_t, dst_transaction_id);	// 目标事务id
	GETSETVAR(base::s_uint32_t, cmd);					// cmd
	GETSETVAR(base::s_uint32_t, cmd_length);			// cmd长度

public:
	AppHeadFrame() {
		is_broadcast = 0;
		src_svr_type = 0;
		dst_svr_type = 0;
		src_instance_id = 0;
		dst_instance_id = 0;
		src_transaction_id = 0;
		dst_transaction_id = 0;
		cmd = 0;
		cmd_length = 0;
	}
};
#ifdef M_PLATFORM_WIN
#pragma pack()
#endif

#ifndef M_SOCKET_IN
#define M_SOCKET_IN  (1)
#endif

#ifndef M_SOCKET_OUT
#define M_SOCKET_OUT (2)
#endif

#ifndef M_SOCKET_DATA
#define M_SOCKET_DATA (3)
#endif

// context
struct TcpSocketContext {
	int fd;
	int msgcount;
	base::s_uint32_t tt;
	netiolib::TcpSocketPtr ptr;
};

struct TcpConnectorContext {
	int fd;
	int msgcount;
	base::s_uint32_t tt;
	netiolib::TcpConnectorPtr ptr;
};

// message
struct TcpSocketMsg {
	netiolib::TcpSocketPtr ptr;
	base::Buffer buf;
	base::s_uint16_t type;
};

struct TcpConnectorMsg {
	netiolib::TcpConnectorPtr ptr;
	SocketLib::SocketError error;
	base::Buffer buf;
	base::s_uint16_t type;
};


struct tag_socket_context_fd{};
struct tag_socket_context_active{};

namespace bmi = boost::multi_index;

// multi index container
typedef bmi::multi_index_container<TcpSocketContext,
			bmi::indexed_by<
				bmi::ordered_unique<bmi::tag<tag_socket_context_fd>, bmi::member<TcpSocketContext, int, &TcpSocketContext::fd> >,
				bmi::ordered_non_unique<bmi::tag<tag_socket_context_active>, bmi::member<TcpSocketContext, base::s_uint32_t, &TcpSocketContext::tt> >
			>
> TcpSocketContextContainer;

typedef bmi::multi_index_container<TcpConnectorContext,
	bmi::indexed_by<
	bmi::ordered_unique<bmi::tag<tag_socket_context_fd>, bmi::member<TcpConnectorContext, int, &TcpConnectorContext::fd> >,
	bmi::ordered_non_unique<bmi::tag<tag_socket_context_active>, bmi::member<TcpConnectorContext, base::s_uint32_t, &TcpConnectorContext::tt> >
	>
> TcpConnectorContextContainer;

// modify class
class FuncModifySocketContext {
public:
	FuncModifySocketContext(int cnt, base::s_uint32_t tt) {
		_cnt = cnt;
		_tt = tt;
	}

	void operator()(TcpSocketContext& ctxt) {
		ctxt.msgcount = _cnt;
		ctxt.tt = _tt;
	}

	void operator()(TcpConnectorContext& ctxt) {
		ctxt.msgcount = _cnt;
		ctxt.tt = _tt;
	}

private:
	int _cnt;
	base::s_uint32_t _tt;
};


#endif