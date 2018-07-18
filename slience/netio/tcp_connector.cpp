#include "slience/netio/netio.hpp"

M_NETIO_NAMESPACE_BEGIN

TcpConnector::TcpConnector(NetIo& netio)
	:TcpStreamSocket<TcpConnector, SocketLib::TcpConnector<SocketLib::IoService> >(netio){
}

SocketLib::TcpConnector<SocketLib::IoService>& TcpConnector::GetSocket() {
	return *this->_socket;
}

bool TcpConnector::Connect(const SocketLib::Tcp::EndPoint& ep, SocketLib::s_uint32_t timeo_sec) {
	try {
		this->_socket->Connect(ep,timeo_sec);
		this->_flag = E_STATE_START;
		this->_remoteep = this->_socket->RemoteEndPoint();
		this->_localep = this->_socket->LocalEndPoint();
		shard_ptr_t<TcpConnector> ref = this->shared_from_this();
		m_function_t<void(SocketLib::s_uint32_t, SocketLib::SocketError)> handler =
			m_bind_t(&TcpConnector::_ReadHandler, ref, placeholder_1, placeholder_2);
		this->_socket->AsyncRecvSome(handler, this->_reader.readbuf, M_SOCKET_READ_SIZE);
		return true;
	}
	catch (SocketLib::SocketError& error) {
		lasterror = error;
		return false;
	}
}

bool TcpConnector::Connect(const std::string& addr, SocketLib::s_uint16_t port, SocketLib::s_uint32_t timeo_sec) {
	SocketLib::Tcp::EndPoint ep(SocketLib::AddressV4(addr), port);
	return Connect(ep,timeo_sec);
}

void TcpConnector::AsyncConnect(const SocketLib::Tcp::EndPoint& ep, SocketLib::SocketError& error) {
	m_function_t<void(SocketLib::SocketError)> handler = m_bind_t(&TcpConnector::_ConnectHandler, this,
		placeholder_1, this->shared_from_this());
	this->_socket->AsyncConnect(handler, ep, error);
	if (error)
		lasterror = error;
}

void TcpConnector::AsyncConnect(const std::string& addr, SocketLib::s_uint16_t port, SocketLib::SocketError& error) {
	SocketLib::Tcp::EndPoint ep(SocketLib::AddressV4(addr), port);
	return AsyncConnect(ep, error);
}

void TcpConnector::_ConnectHandler(const SocketLib::SocketError& error, TcpConnectorPtr conector) {
	if (error) {
		lasterror = error;
		shard_ptr_t<TcpConnector> ref = this->shared_from_this();
		this->_netio.OnConnected(ref, error);
		return;
	}
	try {
		this->_remoteep = this->_socket->RemoteEndPoint();
		this->_localep = this->_socket->LocalEndPoint();
		this->_flag = E_STATE_START;
		shard_ptr_t<TcpConnector> ref = this->shared_from_this();
		this->_netio.OnConnected(ref, error);
		m_function_t<void(SocketLib::s_uint32_t, SocketLib::SocketError)> handler =
			m_bind_t(&TcpConnector::_ReadHandler, ref, placeholder_1, placeholder_2);
		this->_socket->AsyncRecvSome(handler, this->_reader.readbuf, M_SOCKET_READ_SIZE);
	}
	catch (SocketLib::SocketError& err) {
		lasterror = err;
	}
}


M_NETIO_NAMESPACE_END