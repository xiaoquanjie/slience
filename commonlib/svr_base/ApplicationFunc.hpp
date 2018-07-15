#ifndef M_SVR_BASE_APP_FUNC_INCLUDE
#define M_SVR_BASE_APP_FUNC_INCLUDE

#include "slience/base/config.hpp"
#include <string>

extern bool gAppExist;

void OnExitProcess(int);

void CoreFileUnlimit();

void Daemon();

int InitSigHandler();

void SetWorkDir(const std::string& workdir);

std::string GetWorkDir();

int ReadPid(const std::string& pid_file);

int WritePid(const std::string& pid_file);

int KillExist(const std::string& pid_file);

#endif