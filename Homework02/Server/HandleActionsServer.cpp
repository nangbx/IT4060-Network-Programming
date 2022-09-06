#include "HandleActionsServer.h"
#include <string>
#include <vector>
#include "Model.h"
#include "Global.h"
#include "HepersServer.h"
#include <iostream>
#include "Constant.h"




Account parseAccount(std::string data) {

	std::vector<std::string> s = splitString(data, " ");
	if (s.size() != 2 || !isNumber(s[1]) || s[1].size() > 1) {
		std::cout << "Wrong data format!" << "\n";
		exit(0);
	}
	std::string username = s[0];
	bool status = stoi(s[1]);
	return Account(username, status, 0);
}

void getDataOnFile(std::string fileName) {
	std::ifstream myFile(fileName);

	std::string myText;
	while (getline(myFile, myText)) {
		accounts.push_back(parseAccount(myText));
	}

	// Close the file 
	myFile.close();
}
void handleSendData(SOCKET& conn, const std::string data) {

	const char* body = data.c_str();
	int ret;
	ret = send(conn, body, strlen(body), 0);
	if (ret == SOCKET_ERROR) {
		printf("Error %d: Cannot send data.\n", WSAGetLastError());
	}
	Sleep(1);
}

void handleLogin(SOCKET connectedSocket, std::string body, Account& client) {

	if (body == "") {
		handleSendData(connectedSocket, DATA_DOES_NOT_EXIST);
	}
	else {
		if (client.username != "") {
			handleSendData(connectedSocket, LOGGED);
			return;
		}
		// Check account
		EnterCriticalSection(&critical);
		std::vector<Account> accountsT = accounts;
		LeaveCriticalSection(&critical);

		int size = accountsT.size();
		for (int i = 0; i < size; i++) {
			if (accountsT[i].username == body) {
				if (accountsT[i].login) {
					handleSendData(connectedSocket, LOGGED_ANOTHER_DEVICE);
					return;
				}
				else if (!accountsT[i].status) {
					accountsT[i].login = true;
					client = accountsT[i];
					handleSendData(connectedSocket, SUCCESS_LOGIN);
					EnterCriticalSection(&critical);
					accounts = accountsT;
					LeaveCriticalSection(&critical);
					return;
				}
				else {
					handleSendData(connectedSocket, ACCOUNT_LOCKED);
					return;
				}
			}
		}

		handleSendData(connectedSocket, ACCOUNT_DOES_NOT_EXIST);

	}
}

void handlePost(SOCKET connectedSocket, std::string body, Account client) {

	if (body == "") {
		handleSendData(connectedSocket, ::DATA_DOES_NOT_EXIST);
	}
	else {
		if (client.username == "") { // Not logged in
			handleSendData(connectedSocket, NOT_LOGGED_IN);
		}
		else {
			handleSendData(connectedSocket, SUCCESS_POST);
		}
	}
}

void handleBye(SOCKET connectedSocket, Account& client) {
	if (client.username == "") {
		handleSendData(connectedSocket, NOT_LOGGED_IN);
	}
	else {
		EnterCriticalSection(&critical);
		std::vector<Account> accountsT = accounts;
		LeaveCriticalSection(&critical);
		int size = accountsT.size();
		for (int i = 0; i < size; i++) {
			if (accountsT[i].username == client.username) {
				accountsT[i].login = false;
				client.username = "";
				EnterCriticalSection(&critical);
				accounts = accountsT;
				LeaveCriticalSection(&critical);
				handleSendData(connectedSocket, SUCCESS_LOGOUT);
				return;
			}
		}
		handleSendData(connectedSocket, ACCOUNT_DOES_NOT_EXIST);
	}

}

void handleDisconnect(Account& client) {
	EnterCriticalSection(&critical);
	std::vector<Account> accountsT = accounts;
	LeaveCriticalSection(&critical);
	int size = accountsT.size();
	for (int i = 0; i < size; i++) {
		if (accountsT[i].username == client.username) {
			accountsT[i].login = false;
			client.username = "";
			EnterCriticalSection(&critical);
			accounts = accountsT;
			LeaveCriticalSection(&critical);
			return;
		}
	}
}
void getInputServer(int argc, char* argv[], int& port) {
	// Get input port server
	if (argc != 2) {
		printf("Invalid number of parameters\n");
		exit(0);
	}
	char stringPort[6];
	strcpy_s(stringPort, argv[1]);
	port = atoi(stringPort);
}