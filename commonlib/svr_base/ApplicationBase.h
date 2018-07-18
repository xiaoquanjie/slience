#ifndef M_SVR_BASE_APPLICATION_INCLUDE
#define M_SVR_BASE_APPLICATION_INCLUDE

#include <string>
#include "commonlib/svr_base/svrbase.h"
#include <unordered_map>

class ApplicationBase : public netiolib::NetIo {
public:
	ApplicationBase();

	virtual ~ApplicationBase();

	int Init(int argc, char** argv);

	int Run();

	const std::string& ConfigFilePath()const;

	const std::string& AppName()const;

	const std::string& PidFile()const;

	const base::timestamp& GetNow()const;

	netiolib::TcpSocketPtr GetTcpSocketContext(int fd);

	netiolib::TcpConnectorPtr GetTcpConnectorContext(int fd);

protected:
	virtual int OnInit() {
		return 0;
	}

	virtual int OnTick(const base::timestamp& now);

	virtual int OnReload() {
		return 0;
	}

	virtual int OnProc(int fd, const AppHeadFrame& frame, const char* data, base::s_uint32_t data_len) {
		return -1;
	}

	virtual int OnExit() {
		return -1;
	}

	virtual void OnConnection(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error);

	virtual void OnConnection(netiolib::TcpSocketPtr& clisock);

	virtual void OnDisConnection(netiolib::TcpConnectorPtr& clisock);

	virtual void OnDisConnection(netiolib::TcpSocketPtr& clisock);

protected:
	int ParseOpt(int argc, char** argv);

	void Usage()const;

	bool CheckReload();

	void CheckTcpSocketExpire(const base::timestamp& now);

	void OnConnected(netiolib::TcpSocketPtr& clisock) override;

	void OnConnected(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) override;

	void OnDisconnected(netiolib::TcpSocketPtr& clisock) override;

	void OnDisconnected(netiolib::TcpConnectorPtr& clisock) override;

	void OnReceiveData(netiolib::TcpSocketPtr& clisock, SocketLib::Buffer& buffer) override;

	void OnReceiveData(netiolib::TcpConnectorPtr& clisock, SocketLib::Buffer& buffer) override;

protected:
	std::string _workdir;
	std::string _appname;
	std::string _pid_file;
	std::string _conf_file;
	std::string _log_file;
	int _log_level;
	int _log_withpid;
	int _daemon;

	int _msg_cache_size;
	base::timestamp _now;

	// application state
	static bool _app_exit;

	// message list
	base::MutexLock _msg_lock;
	base::slist<TcpSocketMsg*> _tcp_socket_msg_list;
	base::slist<TcpSocketMsg*> _tcp_socket_msg_list2;
	base::slist<TcpConnectorMsg*> _tcp_connector_msg_list;
	base::slist<TcpConnectorMsg*> _tcp_connector_msg_list2;

	// socket container
	TcpSocketContextContainer _tcp_socket_container;
	TcpConnectorContextContainer _tcp_connector_container;
};

#endif