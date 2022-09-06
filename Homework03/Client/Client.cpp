
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <string.h>
#include "ClientMessage.h"
#include "HepersClient.h"
#include "HandleActionsClient.h"
#include "Constant.h"

#pragma comment(lib, "Ws2_32.lib")



int selectFunction(SOCKET& client) {
	char buff[BUFF_SIZE];
	std::string select;
	int option;
	while (1) {
		do {
			printf("!!!Please choose one of the following functions:\n");
			printf("--- 1. Login\n");
			printf("--- 2. Post\n");
			printf("--- 3. Logout\n");
			printf("--- 4. Exit\n");
			printf("Enter your number: ");
			getline(std::cin, select);
			if (select.length() > 3) {
				printf("\n!!!!Incorrect syntax!!!!\n");
				option = -1;
				continue;
			}
			if (select == "") {
				printf("\n!!!!Incorrect syntax!!!!\n");
				option = -1;
				continue;
			}
			else if (!isNumber(select)) {
				printf("\n!!!!Please enter the number!!!!\n");
				continue;
			}
			else {
				option = stoi(select);
			}
		} while (option > 4 && option <= 0);

		switch (option)
		{
		case 1: {
			handleLogin(client, buff);
			break;
		}
		case 2: {
			handlePost(client, buff);
			break;
		}
		case 3: {
			handleBye(client, buff);
			break;
		}
		case 4: {
			printf("See you again!\n");
			printf("Press any key to exit:");
			getchar();
			return 1;
		}
		default:
			break;
		}
	}
	return -1;
}

int main(int argc, char* argv[]) {

	// Inittiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	// Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	//// Get input ip and port server
	int PORT;
	std::string IP_ADDR;
	getInputClient(argc, argv, PORT, IP_ADDR);

	// Specity server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, IP_ADDR.c_str(), &serverAddr.sin_addr);

	// Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot connect server.", WSAGetLastError());
		return 0;
	}

	printf("Connected server!\n");
	printf("<------Welcome to the program------>\n");
	// Communicate with server

	selectFunction(client);


	// Close socket
	closesocket(client);

	// Terminate Winsock
	WSACleanup();

	return 0;
}