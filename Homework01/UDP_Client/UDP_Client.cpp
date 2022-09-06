#include <stdio.h>
#include <iostream>
#include <WINSOCK2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#pragma comment (lib, "Ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 64000

using namespace std;


/**
* @function convertToString: covert char* to string
* @param source: A pointer to a input string
* @param size: variable is equal to size of input string
* 
* return <string>: about the string type data of char* input

*/
string convertToString(char* source, int size) {
	string s = "";
	for (int i = 0; i < size; i++) {
		s = s + source[i];
	}
	return s;
}

int main(int argc, char* argv[]) {
	
	// Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);

	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	// Construct socket
	SOCKET client;

	client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (client == INVALID_SOCKET) {

		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}

	

	// Set time-out for receiving
	//int tv = 10000; // Time-out interval: 10000ms
	//setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char*)(&tv), sizeof(int));

	
	// Get input ip and port server
	if (argc != 3) {
		printf("Invalid number of parameters\n");
		exit(0);
	}
	string ipServer, stringPort;
	ipServer = convertToString(argv[1], strlen(argv[1]));
	stringPort = convertToString(argv[2], strlen(argv[2]));
	int port = atoi(stringPort.c_str());


	printf("Client started!\n");
	// Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	int result = inet_pton(AF_INET, ipServer.c_str(), &serverAddr.sin_addr);
	if (result == 0) {
		printf("Ip address invalid");
		exit(0);
	}


	// Step 4: Communicate with server
	char buff[BUFF_SIZE];
	int ret, serverAddrLen = sizeof(serverAddr);


	while (1) {

		// Send message
		printf("Send to server: ");
		string inputMess;
		getline(cin, inputMess);

		if (inputMess.length() > BUFF_SIZE) {
			printf("Input is larger than buff size\n");
			continue;
		}

		if (inputMess.length() == 0) break;
		

		ret = sendto(client, inputMess.c_str(), strlen(inputMess.c_str()), 0, (sockaddr *)&serverAddr, serverAddrLen);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot send message\n", WSAGetLastError());
		}

		// Receive echo message
		vector<string> arrayIpAddress;
		bool check = true;
		while (true) {
			ret = recvfrom(client, buff, BUFF_SIZE, 0, (sockaddr *)&serverAddr, &serverAddrLen);
			if (ret == SOCKET_ERROR) {
				// Error message
				if (WSAGetLastError() == WSAETIMEDOUT) {
					printf("Time-out!\n");
					check = false;
					break;
				}
				else {
					printf("Error %d: Cannot receive message\n", WSAGetLastError());
					check = false;
					break;
				}
			} else if(strlen(buff) >0){
				buff[ret] = 0;

				// Handling ending character
				if (strcmp(buff, "END") == 0) {
					break;
				}
				else if (buff[0] == '-') {

					// Handling prefix error
					string mess = convertToString(buff, ret);
					mess.erase(0, 1);
					cout << mess << "\n";
					check = false;
					break;
				}
				else {
					string ip = convertToString(buff, ret);
					arrayIpAddress.push_back(ip);
				}
					
			}
		}
		if (check) {
			printf("IP Addresses:\n");
			for (string data : arrayIpAddress) {
				data.erase(0, 1);
				cout << data << "\n";
			}
		}
	}

	// Close socket
	closesocket(client);

	// Terminate WinSock
	WSACleanup();
	return 0;
}