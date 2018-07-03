/*----------------------------------------------------------------
// Copyright (C) 2017 public
//
// 修改人：xiaoquanjie
// 时间：2017/8/10
//
// 修改人：xiaoquanjie
// 时间：
// 修改说明：
//
// 版本：V1.0.0
//----------------------------------------------------------------*/

#ifndef M_ERROR_INCLUDE
#define M_ERROR_INCLUDE

// error code base number
#define M_ERR_BASE_NUM		(2000)
#define M_ERR_BASE_NAG_NUM  (-2000)

// defined
#define M_ERR_SOCKET_OPEN			(M_ERR_BASE_NAG_NUM+1)  // socket is open
#define M_ERR_ENDPOINT_INVALID		(M_ERR_BASE_NAG_NUM+2)  // endpoint is invalid 
#define M_ERR_IOCP_EXIST			(M_ERR_BASE_NAG_NUM+3)  // iocp object is exist
#define M_ERR_IOCP_INVALID			(M_ERR_BASE_NAG_NUM+4)  // iocp object is invalid
#define M_ERR_WIN_ACCEPTEX			(M_ERR_BASE_NAG_NUM+5)  // load AcceptEx function pointer fail
#define M_ERR_WIN_CONNECTEX			(M_ERR_BASE_NAG_NUM+6)  // load ConnectEx function pointer fail
#define M_ERR_EPOLL_EXIST			(M_ERR_BASE_NAG_NUM+7)  // epoll fd is exist
#define M_ERR_ASYNC_CONNECT_FAIL	(M_ERR_BASE_NAG_NUM+8)  // connect fail
#define M_ERR_POSTED_ACCEPT			(M_ERR_BASE_NAG_NUM+9)	// posted accept request
#define M_ERR_POSTED_CONNECT		(M_ERR_BASE_NAG_NUM+10)	// posted connect request
#define M_ERR_POSTED_WRITE			(M_ERR_BASE_NAG_NUM+11)	// posted write request
#define M_ERR_POSTED_READ			(M_ERR_BASE_NAG_NUM+12)	// posted read request
#define M_ERR_ASYNC_ACCEPT_FAIL		(M_ERR_BASE_NAG_NUM+13) // accept fail
#define M_ERR_ASYNC_READ_FAIL		(M_ERR_BASE_NAG_NUM+14) // read fail
#define M_ERR_ASYNC_WRITE_FAIL		(M_ERR_BASE_NAG_NUM+15) // write fail
#define M_ERR_BAD_DESCRIPTOR		(M_ERR_BASE_NAG_NUM+16) // bad descriptor
#define M_ERR_NOT_SERVICE			(M_ERR_BASE_NAG_NUM+17) // not service exist
#define M_ERR_IS_NONBLOCK			(M_ERR_BASE_NAG_NUM+18) // can't convert to block from nonblock
#define M_ERR_TIMEOUT				(M_ERR_BASE_NAG_NUM+19) // 超时

M_SOCKET_NAMESPACE_BEGIN
namespace detail {
	struct error_defined {
		s_int32_t   error;
		const char* msg;
	};
	extern const error_defined gErrorDefined[];
}

#define M_ERR_LAST g_getlasterr()
#define M_MAX_DEFINED_ERR ((s_int32_t)sizeof(detail::gErrorDefined)/(s_int32_t)sizeof(detail::error_defined))
#define M_GET_ERROR_MSG(error,msg) \
	{  s_int32_t erridx = (M_ERR_BASE_NUM+error);\
	   if (erridx>=0 && erridx<M_MAX_DEFINED_ERR)\
			msg = detail::gErrorDefined[erridx].msg; \
	   else\
			msg = g_getlastmsg(error);\
	}

class SocketError {
public:
	SocketError(s_int32_t error);

	SocketError();

	s_int32_t Code()const;

	std::string What()const;

	bool operator!()const;

	operator bool()const;

protected:
	bool operator!=(const SocketError&)const;
	bool operator==(const SocketError&)const;
protected:
	s_int32_t _error;
};

M_SOCKET_NAMESPACE_END

#define M_THROW_SOCKET_ERROR(con,code) \
	if (con) throw SocketError(code);

#define M_THROW_DEFAULT_SOCKET_ERROR(con) \
	if (con) throw SocketError(M_ERR_LAST)

#define M_THROW_DEFAULT_SOCKET_ERROR2(error) \
	if (error) throw error;

#define M_DEFAULT_SOCKET_ERROR(con,error) \
	if (con) error = SocketError(M_ERR_LAST);

#define M_DEFAULT_SOCKET_ERROR2(error) \
	error = SocketError(M_ERR_LAST);

#endif