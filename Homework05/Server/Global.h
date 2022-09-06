#pragma once
#include <process.h>
#include <vector>
#include "Model.h"
#include <Windows.h>

extern std::vector<Client> clients[20];
extern std::vector<WSAEVENT>	events[20];
extern std::vector<Account> accounts;
extern CRITICAL_SECTION section[20];

