// WSAEventSelectServer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include "Model.h"
#include "HepersServer.h"
#include "HandleActionsServer.h"
#include "Global.h"
#include "Constant.h"
#include <process.h>
#pragma comment(lib, "Ws2_32.lib")


#define BUFF_SIZE 2048
#define SERVER_ADDR "127.0.0.1"
#define MAX_CLIENT 1280

std::vector<Account> accounts;
std::vector<Client>  clients[20];
std::vector<WSAEVENT>	events[20];
CRITICAL_SECTION section[20];
int count = 0;


/**
* @function Receive: receive message from client.
*
* @param buff<char* >: buffer receive data.
* @param size<int>: size of buffer.
* @param flags<int>: flag value.
* 
* @return <int>: received data size
**/
int Receive(SOCKET, char *buff, int size, int flags);

/**
* @function Send: Send response data to clients.
*
* @param client<struct Client>: client data.
* @param buff<char* >: data received from client.
* @param ret<int>: received data size.
* @param index<int>: position of client in array.
* @param thread<int>: thread index
**/
void Send(Client client, char *buff, int ret, int index, int thread);

/**
* @function addClient: add new client.
*
* @param sock<SOCKET>: Socket client.
*
**/
void addClient(SOCKET sock);

/**
* @function eraseClient: delete client.
*
* @param client<vector<SOCKET>>: client array of thread.
* @param event<vector<WSAEVENT>: event array of thread.
* @param index<int>: position of client in array.
* @param thread<int>: thread index
*
**/
void eraseClient(std::vector<Client> &client, std::vector<WSAEVENT> &event, int index, int thread);

unsigned __stdcall _thread(void *param) {

	int start = (int)param;
	int thread = start / WSA_MAXIMUM_WAIT_EVENTS;
	
	DWORD	 index;
	DWORD	 nEvents;
	WSANETWORKEVENTS sockEvent;
	std::vector<WSAEVENT> event;
	std::vector<Client> client;

	int ret;
	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];

	while (1) {
		EnterCriticalSection(&section[thread]);
		event = events[thread];
		client = clients[thread];
		LeaveCriticalSection(&section[thread]);
		nEvents = event.size();
		index = WSAWaitForMultipleEvents(nEvents, event.data(), FALSE, 1, FALSE);

		if (index == WSA_WAIT_FAILED) {
			continue;
		}
		if (index == WSA_WAIT_TIMEOUT) {
			continue;
		}
		index = index - WSA_WAIT_EVENT_0;
		
		while (index != nEvents) {
			WSAEnumNetworkEvents(client[index].sock, event[index], &sockEvent);

			WSAResetEvent(event[index]);
			if (sockEvent.lNetworkEvents & FD_READ) {
				//Receive message from client
				if (sockEvent.iErrorCode[FD_READ_BIT] != 0) {
					printf("FD_READ failed with error %d\n", sockEvent.iErrorCode[FD_READ_BIT]);
					continue;
				}

				ret = Receive(client[index].sock, recvBuff, BUFF_SIZE, 0);

				//Release socket and event if an error occurs
				if (ret <= 0) {
					eraseClient(client, event, index, thread);
					nEvents--;
					continue;
				}
				else {									//echo to client
					Send(client[index], recvBuff, ret, index, thread);
					
				}
				WSAEventSelect(client[index].sock, event[index], FD_READ | FD_CLOSE);
			}

			if (sockEvent.lNetworkEvents & FD_CLOSE) {
				if (sockEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
					printf("FD_CLOSE failed with error %d\n", sockEvent.iErrorCode[FD_CLOSE_BIT]);
					continue;
				}
				//Release socket and event

				eraseClient(client, event, index, thread);
				nEvents--;

				continue;
			}
			index++;
		}
	}

	return 0;
}


int main(int argc, char* argv[])
{
	DWORD		nEvents = 0;
	DWORD		index;

	WSANETWORKEVENTS sockEvent;
	//Step 1: Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}
	//Step 2: Construct LISTEN socket	
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
		printf("Error %d: Cannot associate a local address with server socket.", WSAGetLastError());
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN.", WSAGetLastError());
		return 0;
	}

	printf("Server started!\n");

	getDataOnFile("account.txt");

	for (int i = 0; i < 20; i++) {
		InitializeCriticalSection(&section[i]);
	}


	char sendBuff[BUFF_SIZE], recvBuff[BUFF_SIZE];
	SOCKET connSock;
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int ret, i;
	int thread;
	while (1) {
		connSock = accept(listenSock, (sockaddr*)& clientAddr, &clientAddrLen);
		if (count >= MAX_CLIENT) {
			printf("Too many client\n");
			closesocket(connSock);
			continue;
		}

		// add new client
		addClient(connSock);


		if (count % WSA_MAXIMUM_WAIT_EVENTS == 0) {
			_beginthreadex(0, 0, _thread, (void *)count, 0, 0); //start thread
		}
		count++;
	}


	// Delete section
	for (int i = 0; i < 20; i++) {
		DeleteCriticalSection(&section[i]);
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
void Send(Client client, char *buff, int ret, int index, int thread) {
	// Information of a socket
	SOCKET connectedSocket = client.sock;
	std::string sendBuff = client.buff;
	Account accountClient = client.acc;


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
				handleBye(connectedSocket, accountClient, index, thread);
			}
			else {
				handleSendData(connectedSocket, UNKNOWN_MESSAGE);
			}
		}
		else if (pos != std::string::npos) {
			std::string method = mess.substr(0, pos);
			if (method == CLIENT_LOGIN) { // Handle login
				std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
				handleLogin(connectedSocket, body, accountClient, index, thread);
			}
			else if (method == CLIENT_POST) { // Handle post
				std::string body = mess.substr(pos + 1, mess.length() - pos - 1);
				handlePost(connectedSocket, body, accountClient);
			}
			else if (method == CLIENT_BYE) { // Handle bye
				handleBye(connectedSocket, accountClient, index, thread);
			}
			else { // Handle when unknown message
				handleSendData(connectedSocket, UNKNOWN_MESSAGE);
			}

		}
	}
	EnterCriticalSection(&section[thread]);
	clients[thread][index].buff = sendBuff;
	LeaveCriticalSection(&section[thread]);
}


void addClient(SOCKET sock) {
	int thread = count / WSA_MAXIMUM_WAIT_EVENTS;

	EnterCriticalSection(&section[thread]);
	clients[thread].push_back(Client(sock));
	events[thread].push_back(WSACreateEvent());
	LeaveCriticalSection(&section[thread]);

	WSAEventSelect(clients[thread].back().sock, events[thread].back(), FD_READ | FD_CLOSE);
}

void eraseClient(std::vector<Client> &client, std::vector<WSAEVENT> &event, int index, int thread) {
	closesocket(client[index].sock);
	client.erase(client.begin() + index);

	WSACloseEvent(event[index]);
	event.erase(event.begin() + index);

	EnterCriticalSection(&section[thread]);
	clients[thread] = client;
	events[thread] = event;
	LeaveCriticalSection(&section[thread]);
}
