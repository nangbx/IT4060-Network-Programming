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
struct Client {
	Account acc;
	std::string buff;
	SOCKET sock;
	Client(SOCKET _sock) {
		acc = Account();
		sock = _sock;
		buff = "";
	}
};
struct DataThread {
	SOCKET listenSock;
	SOCKET connSock;
	int start;
	DataThread(SOCKET _listen, SOCKET _conn, int _count) {
		listenSock = _listen;
		connSock = _conn;
		start = _count;
	}
};