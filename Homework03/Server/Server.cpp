// SelectTCPServer.cpp : Defines the entry point for the console application.
//

#define FD_SETSIZE 1024
#include <winsock2.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <process.h>
#include <vector>
#include "Model.h"
#include "HepersServer.h"
#include "HandleActionsServer.h"
#include "Global.h"
#include "Constant.h"

#pragma comment (lib,"ws2_32.lib")


#define BUFF_SIZE 2048
#define MAX_CLIENT 10240


SOCKET client[10240];
int count = 0;
CRITICAL_SECTION criticalClient;
CRITICAL_SECTION fd[10];
std::vector<Account> accounts;
DataSock dataClient[10240];
int thread = 0;
std::vector<fd_set> fdThread(10);  //use initfds to initiate readfds at the begining of every loop step

int Receive(SOCKET, char *, int, int);
int Send(SOCKET connectedSocket, char *buff, int ret, int indexClient);

unsigned __stdcall _thread(void *param) {

	// 
	int start = (int)param;
	int thread = start / FD_SETSIZE;
	char rcvBuff[BUFF_SIZE], sendBuff[BUFF_SIZE];
	fd_set readfds;
	FD_ZERO(&readfds);
	int nEvents, ret;

	// Set timeout
	TIMEVAL timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;

	while (1) {
		EnterCriticalSection(&fd[thread]);
		readfds = fdThread[thread];
		LeaveCriticalSection(&fd[thread]);

		nEvents = select(0, &readfds, 0, 0, &timeout);

		if (nEvents < 0) {
			continue;
		}
		//receive data from clients
		for (int i = start; i < FD_SETSIZE + start; i++) {
			if (client[i] == 0)
				continue;

			if (FD_ISSET(client[i], &readfds)) {
				ret = Receive(client[i], rcvBuff, BUFF_SIZE, 0);
				if (ret <= 0) {
					closesocket(client[i]);
					EnterCriticalSection(&fd[thread]);
					FD_CLR(client[i], &fdThread[thread]);
					LeaveCriticalSection(&fd[thread]);
					client[i] = 0;

				}
				else if (ret > 0) {
					Send(client[i], rcvBuff, ret, i);
				}
			}

			if (--nEvents <= 0)
				continue; //no more event
		}

	}
	printf("Exit thread");
	return 0;
}

int main(int argc, char* argv[])
{
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData))
		printf("Version is not supported\n");

	//Step 2: Construct socket	
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	int PORT;
	getInputServer(argc, argv, PORT);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);

	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot bind this address.");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		_getch();
		return 0;
	}

	printf("Server started!\n");

	// Get data on file account.txt
	getDataOnFile("account.txt");

	// Initial section
	InitializeCriticalSection(&criticalClient);
	for (int i = 0; i < 10; i++) {
		InitializeCriticalSection(&fd[i]);
	}
	SOCKET connSock;

	sockaddr_in clientAddr;
	int ret, nEvents, clientAddrLen = sizeof(clientAddr);
	char rcvBuff[BUFF_SIZE], sendBuff[BUFF_SIZE];


	int thread = 0;

	//Step 5: Communicate with clients
	while (1) {
		// New client connection
		connSock = accept(listenSock, (sockaddr*)& clientAddr, &clientAddrLen);

		// Too many clients
		if (count >= MAX_CLIENT) {
			printf("\nToo many clients.\n");
			closesocket(connSock);
			continue;
		}
		client[count] = connSock;
		thread = count / FD_SETSIZE;
		EnterCriticalSection(&fd[thread]);
		FD_SET(connSock, &fdThread[thread]);
		LeaveCriticalSection(&fd[thread]);

		// Create new thread
		if (count % FD_SETSIZE == 0) {
			_beginthreadex(0, 0, _thread, (void *)count, 0, 0); //start thread
		}
		count++;

	}

	// Delete section
	DeleteCriticalSection(&criticalClient);
	for (int i = 0; i < 10; i++) {
		DeleteCriticalSection(&fd[i]);
	}
	// Close socket
	closesocket(listenSock);

	// Terminate winsock
	WSACleanup();
	return 0;
}

/* The recv() wrapper function */
int Receive(SOCKET s, char *buff, int size, int flags) {
	int n;

	n = recv(s, buff, size, flags);
	if (n == SOCKET_ERROR)
		printf("Error: %d", WSAGetLastError());

	return n;
}

/* The send() wrapper function*/
int Send(SOCKET connectedSocket, char *buff, int ret, int indexClient) {
	// Information of a socket
	DataSock dataSock = dataClient[indexClient];
	std::string sendBuff = dataSock.buff;
	Account accountClient = dataSock.sockAcc;


	std::string receiveBuff = "";
	std::string data = convertToString(buff, ret);
	receiveBuff = receiveBuff + data;
	sendBuff = sendBuff + receiveBuff;

	std::vector<std::string> arrayMessage;
	arrayMessage = handleData(sendBuff, ENDING_DELIMITER);


	for (std::string mess : arrayMessage) {
		if (mess == "") {
			handleSendData(connectedSocket, UNKNOWN_MESSAGE);
			continue;
		}
		int pos = mess.find_first_of(" ");
		if (pos == std::string::npos) {
			if (mess == CLIENT_BYE) {
				handleBye(connectedSocket, accountClient, indexClient);
			}
			else {
				handleSendData(connectedSocket, UNKNOWN_MESSAGE);
			}
		}
		else if (pos != std::string::npos) {
			std::string method = mess.substr(0, pos);
			if (method == CLIENT_LOGIN) { // Handle login
				std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
				handleLogin(connectedSocket, body, accountClient, indexClient);
			}
			else if (method == CLIENT_POST) { // Handle post
				std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
				handlePost(connectedSocket, body, accountClient);
			}
			else if (method == CLIENT_BYE) { // Handle bye
				handleBye(connectedSocket, accountClient, indexClient);
			}
			else { // Handle when unknown message
				handleSendData(connectedSocket, UNKNOWN_MESSAGE);
			}

		}
	}
	EnterCriticalSection(&criticalClient);
	dataClient[indexClient].buff = sendBuff;
	LeaveCriticalSection(&criticalClient);
	return ret;
}
