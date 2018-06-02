#ifndef M_SVR_BASE_APPLICATION_INCLUDE
#define M_SVR_BASE_APPLICATION_INCLUDE

#include <string>
#include "slience/base/timer.hpp"

class ApplicationBase {
public:
	ApplicationBase();

	virtual ~ApplicationBase();

	virtual int Init(int argc, char** argv);

	virtual int Run();

	const std::string& ConfigFilePath()const;

	const std::string& AppName()const;

	const std::string& PidFile()const;
protected:
	virtual int OnInit();

	virtual int OnTick(const base::timestamp& now);

	virtual int OnReload();

	virtual int OnProc();

	virtual int OnExit();

protected:
	void CoreFileUnlimit();

	int ParseOpt(int argc, char** argv);

	void Usage()const;

	void Daemon();

protected:
	std::string _appname;
	std::string _pid_file;
	std::string _conf_file;
	std::string _log_file;
	int _log_level;
	int _log_withpid;
	int _daemon;
};

#endif