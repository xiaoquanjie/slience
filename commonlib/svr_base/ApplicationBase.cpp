#include "commonlib/svr_base/ApplicationBase.h"
#include "slience/base/string_util.hpp"
#include "slience/base/logger.hpp"
#include "commonlib/svr_base/getopt.hpp"

ApplicationBase::ApplicationBase() {
	_log_level = base::logger::LOG_LEVEL_TRACE;
}

ApplicationBase::~ApplicationBase() {

}

int ApplicationBase::Init(int argc, char** argv) {
	int ret = 0;
	do {
		ret = ParseOpt(argc, argv);
		if (0 != ret)
			break;

		SetLogFileName(_log_file, true);
		SetLogLevel(_log_level);

	} while (false);

	LogInfo("application init " << _appname);
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

int ApplicationBase::OnInit() {
	return 0;
}

int ApplicationBase::OnTick(const base::timestamp& now) {
	return 0;
}

int ApplicationBase::OnReload() {
	return 0;
}

int ApplicationBase::OnProc() {
	return 0;
}

int ApplicationBase::OnExit() {
	return 0;
}

int ApplicationBase::Run() {
	return 0;
}

int ApplicationBase::ParseOpt(int argc, char** argv) {
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
		{"log_level", 1, &opt_char, 'l'}
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
			}
			break;
		case 'D':
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