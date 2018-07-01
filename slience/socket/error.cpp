#include "slience/socket/config.hpp"
#include "slience/socket/error.hpp"

M_SOCKET_NAMESPACE_BEGIN

namespace detail {
	const error_defined gErrorDefined[] = {
		{ 0,"undefined" },
		{ 1,"socket is open" },
		{ 2,"endpoint is invalid" },
		{ 3,"iocp object existed" },
		{ 4,"iocp object is invalid" },
		{ 5,"load AcceptEx function pointer fail" },
		{ 6,"load ConnectEx function pointer fail" },
		{ 7,"epoll fd existed" },
		{ 8,"connect fail" },
		{ 9,"posted accept request" },
		{ 10,"posted connect request" },
		{ 11,"posted write request" },
		{ 12,"posted read request" },
		{ 13,"accept fail" },
		{ 14,"read fail" },
		{ 15,"write fail" },
		{ 16,"bad descriptor" },
		{ 17,"not service exist" },
		{ 18,"can't convert to block from nonblock" },
		{ 19,"operation time out" }
	};
}

SocketError::SocketError(s_int32_t error) :_error(error) {}

SocketError::SocketError() : _error(0) {}

s_int32_t SocketError::Code()const { return _error; }

std::string SocketError::What()const {
	std::string msg;
	M_GET_ERROR_MSG(_error, msg);
	return msg;
}

bool SocketError::operator !()const {
	return (_error == 0);
}

SocketError::operator bool()const {
	return (_error != 0);
}

M_SOCKET_NAMESPACE_END