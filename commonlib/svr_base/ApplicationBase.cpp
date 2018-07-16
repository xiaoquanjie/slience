#include "commonlib/svr_base/ApplicationBase.h"
#include "slience/base/string_util.hpp"
#include "slience/base/logger.hpp"
#include "commonlib/svr_base/getopt.hpp"
#include "commonlib/svr_base/ApplicationFunc.hpp"

#include "mysql_wrapper/mysql_wrapper.h"
#include "protolib/src/svr_base.pb.h"
#include "protolib/src/cmd.pb.h"

/////////////////////////////////////////////////////////////

ApplicationBase::ApplicationBase() {
	_log_level = base::logger::LOG_LEVEL_TRACE;
	_log_withpid = 0;
	_daemon = 0;
	_msg_cache_size = 5000;
}

ApplicationBase::~ApplicationBase() {

}

int ApplicationBase::Init(int argc, char** argv) {
	int ret = 0;
	do {
		// set core file unlimit
		CoreFileUnlimit();

		ret = ParseOpt(argc, argv);
		if (0 != ret)
			break;

		// set work dir
		SetWorkDir(_workdir);

		// can't start two process
		ret = KillExist(_pid_file);
		if (ret != 0) {
			printf("can't kill old process\n");
			exit(0);
		}

		// set daemon
		if (_daemon == 1) {
			Daemon();
		}

		// init signal
		InitSigHandler();

		// init log
		SetLogFileName(_log_file, (bool)_log_withpid);
		SetLogLevel(_log_level);

		ret = WritePid(_pid_file);
		if (0 != ret) {
			break;
		}

		ret = OnReload();
		if (ret != 0) {
			LogError("reload error........");
			break;
		}

		ret = OnInit();
		if (ret != 0) {
			LogError("init error.....");
			break;
		}
	} while (false);
	   
	LogInfo("application param : _workdir=" << GetWorkDir());
	LogInfo("application param : _appname=" << _appname);
	LogInfo("application param : _log_file=" << _log_file);
	LogInfo("application param : _log_level=" << _log_level);
	
	return ret;
}

const std::string& ApplicationBase::ConfigFilePath()const {
	return _conf_file;
}

const std::string& ApplicationBase::AppName()const {
	return _appname;
}

const std::string& ApplicationBase::PidFile()const {
	return _pid_file;
}

const base::timestamp& ApplicationBase::GetNow()const {
	return _now;
}

TcpSocketContext* ApplicationBase::GetTcpSocketContext(int fd) {
	std::unordered_map<int, TcpSocketContext>::iterator iter = _tcp_socket_map.find(fd);
	if (iter != _tcp_socket_map.end()) {
		return &(iter->second);
	}
	else {
		return 0;
	}
}

TcpConnectorContext* ApplicationBase::GetTcpConnectorContext(int fd) {
	std::unordered_map<int, TcpConnectorContext>::iterator iter = _tcp_connector_map.find(fd);
	if (iter != _tcp_connector_map.end()) {
		return &(iter->second);
	}
	else {
		return 0;
	}
}

int ApplicationBase::Run() {
	base::slist<TcpSocketMsg*> tmp_tcp_socket_msg_list;
	base::slist<TcpSocketMsg*> tmp_tcp_socket_msg_list2;
	base::slist<TcpConnectorMsg*> tmp_tcp_connector_msg_list;
	base::slist<TcpConnectorMsg*> tmp_tcp_connector_msg_list2;

	while (!gAppExist) {
		base::timestamp t_now;
		if (t_now.millisecond() > _now.millisecond()) {
			_now = t_now;
			OnTick(_now);
		}
		if (CheckReload()) {
			LogInfo("reload begin.................");
			OnReload();
			LogInfo("reload end....................");
		}

		if (_tcp_socket_msg_list.size() || _tcp_connector_msg_list.size()) {
			_msg_lock.lock();
			tmp_tcp_socket_msg_list.swap(_tcp_socket_msg_list);
			tmp_tcp_connector_msg_list.swap(_tcp_connector_msg_list);
			_msg_lock.unlock();
		}	

		while (tmp_tcp_connector_msg_list.size()) {
			TcpConnectorMsg* pmsg = tmp_tcp_connector_msg_list.front();
			if (pmsg->type == M_SOCKET_DATA) {
				AppHeadFrame* pFrame = (AppHeadFrame*)pmsg->buf.Data();
				const char* data = (const char*)pmsg->buf.Data() + sizeof(AppHeadFrame);
				OnProc(pmsg->ptr->GetSocket().GetFd(), *pFrame, data, pFrame->get_cmd_length());
			}
			else if (pmsg->type == M_SOCKET_IN) {
				OnConnection(pmsg->ptr, pmsg->error);
			} 
			else if (pmsg->type == M_SOCKET_OUT) {
				OnDisConnection(pmsg->ptr);
			}
			else {
				assert(0);
			}
			tmp_tcp_connector_msg_list.pop_front();
			tmp_tcp_connector_msg_list2.push_back(pmsg);
		}
		while (tmp_tcp_socket_msg_list.size()) {
			TcpSocketMsg* pmsg = tmp_tcp_socket_msg_list.front();
			if (pmsg->type == M_SOCKET_DATA) {
				TcpSocketContext* context = GetTcpSocketContext(pmsg->ptr->GetSocket().GetFd());
				if (context) {
					context->msgcount += 1;
				}
				AppHeadFrame* pFrame = (AppHeadFrame*)pmsg->buf.Data();
				const char* data = (const char*)pmsg->buf.Data() + sizeof(AppHeadFrame);
				OnProc(pmsg->ptr->GetSocket().GetFd(), *pFrame, data, pFrame->get_cmd_length());
			}
			else if (pmsg->type == M_SOCKET_IN) {
				OnConnection(pmsg->ptr);
			}
			else if (pmsg->type == M_SOCKET_OUT) {
				OnDisConnection(pmsg->ptr);
			}
			else {
				assert(0);
			}
			tmp_tcp_socket_msg_list.pop_front();
			tmp_tcp_socket_msg_list2.push_back(pmsg);
		}

		if (tmp_tcp_connector_msg_list2.size() || tmp_tcp_socket_msg_list2.size()) {
			_msg_lock.lock();
			// tmp_tcp_connector_msg_list2
			if ((tmp_tcp_connector_msg_list2.size() + _tcp_connector_msg_list2.size()) <= _msg_cache_size) {
				_tcp_connector_msg_list2.join(tmp_tcp_connector_msg_list2);
			}
			else {
				while (tmp_tcp_connector_msg_list2.size()) {
					if (_tcp_connector_msg_list2.size() < _msg_cache_size) {
						_tcp_connector_msg_list2.push_back(tmp_tcp_connector_msg_list2.front());
					}
					else {
						delete tmp_tcp_connector_msg_list2.front();
					}
					tmp_tcp_connector_msg_list2.pop_front();
				}

			}
			// tmp_tcp_socket_msg_list2
			if ((tmp_tcp_socket_msg_list2.size() + _tcp_socket_msg_list2.size()) <= _msg_cache_size) {
				_tcp_socket_msg_list2.join(tmp_tcp_socket_msg_list2);
			}
			else {
				while (tmp_tcp_socket_msg_list2.size()) {
					if (_tcp_socket_msg_list2.size() < _msg_cache_size) {
						_tcp_socket_msg_list2.push_back(tmp_tcp_socket_msg_list2.front());
					}
					else {
						delete tmp_tcp_socket_msg_list2.front();
					}
					tmp_tcp_socket_msg_list2.pop_front();
				}
			}
			_msg_lock.unlock();
		}
		else {
			Sleep(1);
		}
	}
	OnExit();
	LogInfo("application exit..................");
	return 0;
}

int ApplicationBase::ParseOpt(int argc, char** argv) {
	_workdir = base::StringUtil::directory(argv[0]);
	_appname = base::StringUtil::basename(argv[0]);
	_appname = base::StringUtil::remove_from_end(_appname, ".exe");
	_pid_file = _appname + ".pid";

	if (argc < 2) {
		Usage();
		exit(0);
	}

	static int opt_char = 0;
	static base::Option long_option[] = {
		{"config_file", 1, &opt_char, 'C'},
		{"help", 0, 0, 'h'},
		{"daemon", 0, 0, 'D'},
		{"log_file", 1, &opt_char, 'L'},
		{"log_level", 1, &opt_char, 'l'},
		{"log_withpid", 1, &opt_char, 'p'}
	};

	int opt_idx = 0;
	int opt;
	while (-1 != (opt = base::GetOptLong(argc, argv, "DHh", long_option, &opt_idx))) {
		switch (opt) {
		case 0:
			switch (*long_option[opt_idx].flag) {
			case 'C':
				_conf_file = base::GetOptArg();
				break;
			case 'L':
				_log_file = base::GetOptArg();
				break;
			case 'l':
				_log_level = base::GetOptArgI();
				break;
			case 'p':
				_log_withpid = base::GetOptArgI();
				break;
			}
			break;
		case 'D':
			_daemon = 1;
			break;
		case 'H':
		case 'h':
		case '?':
		default:
			Usage();
			exit(0);
			break;
		}
	}
	
	if (_log_file.empty()) {
		_log_file = _appname;
	}

	return 0;
}

void ApplicationBase::Usage()const {
	printf("Usage:\n");
	printf("required arguments:\n");
	printf("--config_file		the config file path\n");

	printf("\noptional arguments:\n");
	printf("--log_file		the log file path\n");
	printf("--log_level		the log level\n");
}

bool ApplicationBase::CheckReload() {
	static base::timestamp last_check_time;
	if ((GetNow().second() - last_check_time.second()) > 2) {
		last_check_time = GetNow();

		static const char* filename = "_reload_";
		FILE *fp = myfopen(filename, "r", _SH_DENYNO);
		if (fp) {
			fclose(fp);
			remove(filename);
			return true;
		}
	}
	return false;
}

void ApplicationBase::OnConnected(netiolib::TcpSocketPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_IN;
	_tcp_socket_msg_list.push_back(pMessage);
}

void ApplicationBase::OnConnected(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) {
	base::ScopedLock scoped(_msg_lock);
	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->error = error;
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_IN;
	_tcp_connector_msg_list.push_back(pMessage);
}

void ApplicationBase::OnDisconnected(netiolib::TcpSocketPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}
;
	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_OUT;
	_tcp_socket_msg_list.push_back(pMessage);
}

void ApplicationBase::OnDisconnected(netiolib::TcpConnectorPtr& clisock) {
	base::ScopedLock scoped(_msg_lock);
	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_OUT;
	_tcp_connector_msg_list.push_back(pMessage);
}

void ApplicationBase::OnReceiveData(netiolib::TcpSocketPtr& clisock, SocketLib::Buffer& buffer) {
	base::ScopedLock scoped(_msg_lock);
	TcpSocketMsg* pMessage = 0;
	if (_tcp_socket_msg_list2.size() > 0) {
		pMessage = _tcp_socket_msg_list2.front();
		_tcp_socket_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpSocketMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_DATA;
	pMessage->buf.Write(buffer.Data(), buffer.Length());
	_tcp_socket_msg_list.push_back(pMessage);
}

void ApplicationBase::OnReceiveData(netiolib::TcpConnectorPtr& clisock, SocketLib::Buffer& buffer) {
	base::ScopedLock scoped(_msg_lock);
	TcpConnectorMsg* pMessage = 0;
	if (_tcp_connector_msg_list2.size() > 0) {
		pMessage = _tcp_connector_msg_list2.front();
		_tcp_connector_msg_list2.pop_front();
	}
	else {
		pMessage = new TcpConnectorMsg;
	}

	pMessage->ptr = clisock;
	pMessage->type = M_SOCKET_DATA;
	pMessage->buf.Write(buffer.Data(), buffer.Length());
	_tcp_connector_msg_list.push_back(pMessage);
}

void ApplicationBase::OnConnection(netiolib::TcpConnectorPtr& clisock, SocketLib::SocketError error) {
	if (!error) {
		// connect success
		int fd = clisock->GetSocket().GetFd();
		TcpConnectorContext context;
		context.ptr = clisock;
		context.msgcount = 0;
		context.tt = GetNow();
		_tcp_connector_map[fd] = context;
		LogInfo("new connection, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << context.tt.format_ymd_hms());
	
		proto::SocketClientIn client_in;
		std::string str = client_in.SerializeAsString();
		AppHeadFrame frame;
		frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_IN);
		OnProc(fd, frame, str.c_str(), str.size());
	}
}

void ApplicationBase::OnConnection(netiolib::TcpSocketPtr& clisock) {
	int fd = clisock->GetSocket().GetFd();
	TcpSocketContext context;
	context.ptr = clisock;
	context.msgcount = 0;
	context.tt = GetNow();
	_tcp_socket_map[fd] = context;

	LogInfo("new connection, remote_ip: " << clisock->RemoteEndpoint().Address() <<" fd: " << fd << " time: " << context.tt.format_ymd_hms());
	proto::SocketClientIn client_in;
	std::string str = client_in.SerializeAsString();
	AppHeadFrame frame;
	frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_IN);
	OnProc(fd, frame, str.c_str(), str.size());
}

void ApplicationBase::OnDisConnection(netiolib::TcpConnectorPtr& clisock) {
	int fd = clisock->GetSocket().GetFd();
	proto::SocketClientOut client_out;
	std::string str = client_out.SerializeAsString();
	AppHeadFrame frame;
	frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_OUT);

	LogInfo("connection break, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
	OnProc(fd, frame, str.c_str(), str.size());
	if (0 == _tcp_connector_map.erase(fd)) {
		LogError("this is a bug!!!!!!!!!!!!!!!!!");
	}
}

void ApplicationBase::OnDisConnection(netiolib::TcpSocketPtr& clisock) {
	int fd = clisock->GetSocket().GetFd();
	proto::SocketClientOut client_out;
	std::string str = client_out.SerializeAsString();
	AppHeadFrame frame;
	frame.set_cmd(proto::CMD::CMD_SOCKET_CLIENT_OUT);
	
	LogInfo("connection break, remote_ip: " << clisock->RemoteEndpoint().Address() << " fd: " << fd << " time: " << GetNow().format_ymd_hms());
	OnProc(fd, frame, str.c_str(), str.size());
	if (0 == _tcp_socket_map.erase(fd)) {
		LogError("this is a big bug!!!!!!!!!!!!!!!!!");
	}
}