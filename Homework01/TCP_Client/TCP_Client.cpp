
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <string.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 64000
#define ENDING_DELIMITER "\r\n"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

/**
* @function convertToString: covert char* to string
* @param source: A pointer to a input string
* @param size: variable is equal to size of input string
*
* return <string>: about the string type data of char* input

*/
string convertToString(char* a, int size) {
	string s = "";
	for (int i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
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

	// Get input ip and port server
	if (argc != 3) {
		printf("Invalid number of parameters\n");
		exit(0);
	}
	string ipServer, stringPort;
	ipServer = convertToString(argv[1], strlen(argv[1]));
	stringPort = convertToString(argv[2], strlen(argv[2]));
	int port = atoi(stringPort.c_str());

	
	// Specity server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ipServer.c_str(), &serverAddr.sin_addr);
	
	// Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error %d: Cannot connect server.", WSAGetLastError());
		return 0;
	}

	printf("Connected server!\n");

	// Communicate with server
	char buff[BUFF_SIZE];
	int ret;
	while (1) {
		// Get message
		printf("Send to server: ");
		string mess;
		getline(cin, mess);

		// Close socket when input is empty string
		if (mess.length() == 0) break;

		if (mess.length() > BUFF_SIZE) {
			printf("Input is larger than buff size\n");
			continue;
		}
		// Add ending delimiter to message
		mess = mess + ENDING_DELIMITER;
		
		// Send message
		do
		{
			string sendMess = "";
			if (mess.length() < BUFF_SIZE) { // When length of mess < BUFF_SIZE
				sendMess = mess;
			}
			else { // When length of mess > BUFF_SIZE
				sendMess = mess.substr(0, BUFF_SIZE);
			}
			ret = send(client, sendMess.c_str(), strlen(sendMess.c_str()), 0);
			mess.erase(0, sendMess.length());
		}while (mess.length() != 0);
		

		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send data.", WSAGetLastError());
		}

		// Receive echo message
		string rBuff = "";
		while (rBuff.find(ENDING_DELIMITER) == string::npos) {
			ret = recv(client, buff, BUFF_SIZE, 0);
			if (ret == SOCKET_ERROR) {
				printf("Error %d: Cannot receive data.", WSAGetLastError());
				break;
			}
			rBuff = rBuff + convertToString(buff, ret);
		}

		// Split prefix
		rBuff.erase(0, 1);
		printf("%s\n", rBuff.c_str());
	}

	// Close socket
	closesocket(client);

	// Terminate Winsock
	WSACleanup();

	return 0;
}