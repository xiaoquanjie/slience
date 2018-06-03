#ifndef M_SVR_BASE_APPLICATION_INCLUDE
#define M_SVR_BASE_APPLICATION_INCLUDE

#include <string>
#include "slience/base/timer.hpp"

class ApplicationBase {
public:
	ApplicationBase();

	virtual ~ApplicationBase();

	int Init(int argc, char** argv);

	int Run();

	const std::string& ConfigFilePath()const;

	const std::string& AppName()const;

	const std::string& PidFile()const;

	const base::timestamp& Now()const;

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
};

#endif