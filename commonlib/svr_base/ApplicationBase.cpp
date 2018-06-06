#include "commonlib/svr_base/ApplicationBase.h"
#include "slience/base/string_util.hpp"
#include "slience/base/logger.hpp"
#include "commonlib/svr_base/getopt.hpp"
#include "commonlib/svr_base/ApplicationFunc.hpp"

#include "mysql_wrapper/mysql_wrapper.h"
#include "protolib/src/routersvr_config.pb.h"

/////////////////////////////////////////////////////////////

ApplicationBase::ApplicationBase() {
	_log_level = base::logger::LOG_LEVEL_TRACE;
	_log_withpid = 0;
	_daemon = 0;
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
	
	SqlConnectionPtr conn_ptr = MysqlWrapper::GetConnect("", "", "", 0);

	config::RouterSvrConfig config;
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

const base::timestamp& ApplicationBase::Now()const {
	return _now;
}

int ApplicationBase::Run() {
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

		OnProc();
		Sleep(1);
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
	if ((Now().second() - last_check_time.second()) > 2) {
		last_check_time = Now();

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

///////////////////////////////////////////////////////////////////
