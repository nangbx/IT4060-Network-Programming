#pragma once
#include <string>

struct Account {
	std::string username;
	bool status;
	bool login;
	Account(std::string _username, bool _status, bool _login) {
		username = _username;
		status = _status;
		login = _login;
	}
	Account() {
		username = "";
		status = 1;
		login = 0;
	}
};
