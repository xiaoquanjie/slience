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

protected:
	virtual int OnInit() {
		return 0;
	}

	virtual int OnTick(const base::timestamp& now) {
		return -1;
	}

	virtual int OnReload() {
		return 0;
	}

	virtual int OnProc() {
		return -1;
	}

	virtual int OnExit() {
		return -1;
	}

protected:
	int ParseOpt(int argc, char** argv);

	void Usage()const;

	bool CheckReload();

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

	base::timestamp _now;

	// application state
	static bool _app_exit;

	// message list
	base::MutexLock _msg_lock;
	base::slist<TcpSocketMsg*> _tcp_socket_msg;
	base::slist<TcpSocketMsg*> _tcp_socket_msg2;
	base::slist<TcpConnectorMsg*> _tcp_connector_msg;
	base::slist<TcpConnectorMsg*> _tcp_connector_msg2;

	// socket map
	std::unordered_map<int, TcpSocketContext> _tcp_socket_map;
	std::unordered_map<int, TcpConnectorContext> _tcp_connector_map;
};

#endif