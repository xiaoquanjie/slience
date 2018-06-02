#include "commonlib/svr_base/ApplicationBase.h"
#include "slience/base/string_util.hpp"
#include "slience/base/logger.hpp"
#include "commonlib/svr_base/getopt.hpp"

#ifndef M_PLATFORM_WIN
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/param.h>
#endif


bool ApplicationBase::_app_exit = false;

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
		SetWorkDir();

		// can't start two process
		ret = KillExist();
		if (ret != 0) {
			printf("can't kill old process\n");
			exit(0);
		}

		// set daemon
		Daemon();

		// init signal
		InitSigHandler();

		// init log
		SetLogFileName(_log_file, (bool)_log_withpid);
		SetLogLevel(_log_level);

		ret = WritePid();
		if (0 != ret) {
			break;
		}

		ret = OnInit();
		if (ret != 0) {
			LogError("init error.....");
			break;
		}

	} while (false);

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

int ApplicationBase::Run() {
	while (!_app_exit) {
		OnProc();
		Sleep(50);
	}
	OnExit();
	LogInfo("application exit..................");
	return 0;
}

void ApplicationBase::CoreFileUnlimit() {
#ifndef M_PLATFORM_WIN
	struct rlimit rlim;
	struct rlimit rlim_new;
	if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_CORE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);
		}
	}
#endif
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

void ApplicationBase::SetWorkDir() {
#ifdef M_PLATFORM_WIN
	SetCurrentDirectoryA(_workdir.c_str());
#else
	chdir(_workdir.c_str());
#endif
}

void ApplicationBase::Daemon() {
	if (_daemon != 1)
		return;

#ifndef M_PLATFORM_WIN
	pid_t pid = fork();
	if (pid != 0) 
		exit(0);

	// ÍÑÀëÖÕ¶Ë
	setsid();

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	pid = fork();

	if (pid != 0)
		exit(0);

	for (int i = 0; i < NOFILE; i++)
		close(i);

	umask(0);
#endif
}

int ApplicationBase::ReadPid() {
	int pid = -1;
	FILE* fp = myfopen(_pid_file.c_str(), "rt", _SH_DENYRW);
	if (!fp) {
		LogWarn("read pid error, can't open file=" << _pid_file);
		return -1;
	}

	myfscanf(fp, "%d", &pid);
	fclose(fp);
	return pid;
}

int ApplicationBase::WritePid() {
	FILE* fp = myfopen(_pid_file.c_str(), "wt", _SH_DENYRW);
	if (!fp) {
		LogWarn("write pid error, can't open file=" << _pid_file);
		return -1;
	}

	fprintf(fp, "%d\n", static_cast<int>(GetCurrentProcessId()));
	fflush(fp);
	fclose(fp);
	return 0;
}

int ApplicationBase::InitSigHandler() {
#ifndef M_PLATFORM_WIN
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = OnExitProcess;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
#endif
	return 0;
}

int ApplicationBase::KillExist() {
	int pid = ReadPid();
	if (-1 == pid)
		return 0;

#ifdef M_PLATFORM_WIN
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (hProcess == NULL) {
		printf("OpenProcess fail, pid=%d\n", pid);
		return 0;
	}
	if (!TerminateProcess(hProcess, 0)) {
		return -1;
	}
	WaitForSingleObject(hProcess, INFINITE);
	printf("kill old process over\n");
#else
	kill((pid_t)pid, SIGQUIT);
#endif

	return 0;
}

void ApplicationBase::OnExitProcess(int) {
	_app_exit = true;
}