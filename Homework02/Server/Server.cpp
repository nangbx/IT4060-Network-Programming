#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <process.h>
#include <fstream>
#include "HepersServer.h"
#include "Global.h"
#include "HandleActionsServer.h"
#include "Constant.h"
#pragma comment(lib, "Ws2_32.lib")



// Variable global
CRITICAL_SECTION critical;
std::vector<Account> accounts;

/* echoThread - Thread to receive the message from client and echo*/
unsigned __stdcall echoThread(void *param) {
	char buff[BUFF_SIZE];
	int ret;

	SOCKET connectedSocket = (SOCKET)param;
	std::string sendBuff = ""; // send buffer
	std::string receiveBuff; // receiver buffer

	Account accountClient;
	
	
	std::vector<std::string> arrayMessage;
	// Receive message from client
	while (1) {
		receiveBuff = "";
		ret = recv(connectedSocket, buff, BUFF_SIZE, 0);

		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
			break;
		}
		else if (ret == 0) {
			printf("Client disconnects.\n");
			break;
		}
		else {
			std::string data = convertToString(buff, ret);
			receiveBuff = receiveBuff + data;
			sendBuff = sendBuff + receiveBuff;
			

			arrayMessage = handleData(sendBuff, ENDING_DELIMITER);

			for (std::string mess : arrayMessage) {
				if (mess == "") {
					handleSendData(connectedSocket, UNKNOWN_MESSAGE);
					continue;
				}
				int pos = mess.find_first_of(" ");
				if (pos == std::string::npos) {
					if (mess == CLIENT_BYE) {
						handleBye(connectedSocket, accountClient);
					}
					else {
						handleSendData(connectedSocket, UNKNOWN_MESSAGE);
					}
				}
				else if (pos != std::string::npos) {
					std::string method = mess.substr(0, pos);
					if (method == CLIENT_LOGIN) { // Handle login
						std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
						handleLogin(connectedSocket, body, accountClient);
					}
					else if (method == CLIENT_POST) { // Handle post
						std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
						handlePost(connectedSocket, body, accountClient);
					}
					else if (method == CLIENT_BYE) { // Handle bye
						handleBye(connectedSocket, accountClient);
					}
					else { // Handle when unknown message
						handleSendData(connectedSocket, UNKNOWN_MESSAGE);
					}
					
				}
			}
		}
	}

	// Handle when client disconnect
	handleDisconnect(accountClient); 
	closesocket(connectedSocket);
	return 0;
}

int main(int argc, char* argv[]) {

	int PORT;

	// Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	// Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock == INVALID_SOCKET) {
		
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	// Get input port
	getInputServer(argc, argv, PORT);

	// Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	// Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN", WSAGetLastError());
		return 0;
	}
	printf("Server started!\n");

	// Get data file account
	getDataOnFile("D:\\account.txt");
	
	// Initialize the critical section 
	InitializeCriticalSection(&critical);
	SOCKET connSocket;
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	while (1) {
		while (1) {

			connSocket = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
			if (connSocket == SOCKET_ERROR)
				printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
			else {
				_beginthreadex(0, 0, echoThread, (void *)connSocket, 0, 0); //start thread
			}
		}
		
		
	}
	
	// Release resources used by the critical section object
	DeleteCriticalSection(&critical);
	// Close listen sock
	closesocket(listenSock);
	
	// Terminate winsock
	WSACleanup();
	getchar();
	return 0;
}
