#include "slience/netio/netio.hpp"

M_NETIO_NAMESPACE_BEGIN

HttpConnector::HttpConnector(NetIo& netio)
	:HttpBaseSocket<HttpConnector, SocketLib::TcpConnector<SocketLib::IoService>
	, HttpCliRecvMsg>(netio) {
}

SocketLib::TcpConnector<SocketLib::IoService>& HttpConnector::GetSocket() {
	return (*this->_socket);
}

bool HttpConnector::Connect(const SocketLib::Tcp::EndPoint& ep) {
	try {
		this->_socket->Connect(ep);
		return true;
	}
	catch (SocketLib::SocketError& error) {
		lasterror = error;
		return false;
	}
}

bool HttpConnector::Connect(const std::string& addr, SocketLib::s_uint16_t port) {
	SocketLib::Tcp::EndPoint ep(SocketLib::AddressV4(addr), port);
	return Connect(ep);
}

void HttpConnector::AsyncConnect(const SocketLib::Tcp::EndPoint& ep) {
	try {
		m_function_t<void(SocketLib::SocketError)> handler = m_bind_t(&HttpConnector::_ConnectHandler, this,
			placeholder_1, this->shared_from_this());
		this->_socket->AsyncConnect(handler, ep);
	}
	catch (SocketLib::SocketError& error) {
		lasterror = error;
	}
}

void HttpConnector::AsyncConnect(const std::string& addr, SocketLib::s_uint16_t port) {
	SocketLib::Tcp::EndPoint ep(SocketLib::AddressV4(addr), port);
	return AsyncConnect(ep);
}

void HttpConnector::_ConnectHandler(const SocketLib::SocketError& error, HttpConnectorPtr conector) {
	if (error) {
		lasterror = error;
		shard_ptr_t<HttpConnector> ref = this->shared_from_this();
		this->_netio.OnConnected(ref, error);
		return;
	}
	try {
		this->_remoteep = this->_socket->RemoteEndPoint();
		this->_localep = this->_socket->LocalEndPoint();
		this->_flag = E_STATE_START;
		shard_ptr_t<HttpConnector> ref = this->shared_from_this();
		this->_netio.OnConnected(ref, error);
		m_function_t<void(SocketLib::s_uint32_t, SocketLib::SocketError)> handler =
			m_bind_t(&HttpConnector::_ReadHandler, ref, placeholder_1, placeholder_2);
		this->_socket->AsyncRecvSome(handler, this->_reader.readbuf, M_SOCKET_READ_SIZE);
	}
	catch (SocketLib::SocketError& err) {
		lasterror = err;
	}
}

M_NETIO_NAMESPACE_END