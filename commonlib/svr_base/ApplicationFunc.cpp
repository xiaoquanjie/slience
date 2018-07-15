#include "commonlib/svr_base/ApplicationFunc.hpp"
#include "slience/base/string_util.hpp"
#include "slience/base/logger.hpp"

#ifndef M_PLATFORM_WIN
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/param.h>
#endif

#include "slience/base/win.hpp"

bool gAppExist = false;

void OnExitProcess(int) {
	gAppExist = true;
}

void CoreFileUnlimit() {
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

void Daemon() {
#ifndef M_PLATFORM_WIN
	pid_t pid = fork();
	if (pid != 0)
		exit(0);

	// Õ—¿Î÷’∂À
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

int InitSigHandler() {
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

void SetWorkDir(const std::string& workdir) {
#ifdef M_PLATFORM_WIN
	SetCurrentDirectoryA(workdir.c_str());
#else
	chdir(workdir.c_str());
#endif
}

std::string GetWorkDir() {
	char work_dir[512] = {};
#ifdef M_PLATFORM_WIN
	GetCurrentDirectoryA(512, work_dir);
#else
	getcwd(work_dir, 512);
#endif
	return std::string(work_dir);
}

int ReadPid(const std::string& pid_file) {
	int pid = -1;
	FILE* fp = myfopen(pid_file.c_str(), "rt", _SH_DENYRW);
	if (!fp) {
		LogWarn("read pid error, can't open file=" << pid_file);
		return -1;
	}

	myfscanf(fp, "%d", &pid);
	fclose(fp);
	return pid;
}

int WritePid(const std::string& pid_file) {
	FILE* fp = myfopen(pid_file.c_str(), "wt", _SH_DENYRW);
	if (!fp) {
		LogWarn("write pid error, can't open file=" << pid_file);
		return -1;
	}

	fprintf(fp, "%d\n", static_cast<int>(GetCurrentProcessId()));
	fflush(fp);
	fclose(fp);
	return 0;
}

int KillExist(const std::string& pid_file) {
	int pid = ReadPid(pid_file);
	if (-1 == pid)
		return 0;

#ifdef M_PLATFORM_WIN
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (hProcess == NULL) {
		printf("OpenProcess fail, pid=%d\n", pid);
		return 0;
	}
	if (!TerminateProcess(hProcess, 0)) {
		return 0;
	}
	WaitForSingleObject(hProcess, INFINITE);
	printf("kill old process over\n");
#else
	kill((pid_t)pid, SIGQUIT);
#endif

	return 0;
}
