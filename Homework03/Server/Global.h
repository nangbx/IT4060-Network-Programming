#pragma once
#include <process.h>
#include <vector>
#include "Model.h"
#include <Windows.h>

extern SOCKET client[10240];
extern CRITICAL_SECTION criticalClient;
extern std::vector<Account> accounts;
extern DataSock dataClient[10240];
