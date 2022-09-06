#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string.h>
#include <vector>

#define SERVER_ADDR "127.0.0.1"
#define PORT 5500
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

/**
* @function split: split string by character delimiter
* @param sBuff: string to split
* @param delimiter: string delimiter
*
* return <vector<string>>: vector substrings after being split

*/
vector<string> split(string& sBuff, string delimiter) {
	vector<string> out;
	int pos = 0;
	while ((pos = sBuff.find(delimiter)) != string::npos) {
		out.push_back(sBuff.substr(0, pos));
		sBuff.erase(0, pos + delimiter.length());
	}
	return out;
}

/**
* @function findSum: find sum of characters in string
* @param str: string to process
*
* return <int>: -1 if string contains non-numeric characters
		  sum of characters in string 
*/
int findSum(string str) {
	int sum = 0;
	int size = str.size();
	for (int i = 0; i < size; i++) {
		if (!isdigit(str[i])) {
			return -1;
		}
		else {
			sum += str[i] - '0';
		}
	}
	return sum;
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
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSock == INVALID_SOCKET) {
		printf("Error %d: Cannot create server socket.", WSAGetLastError());
		return 0;
	}
	// Get input port server
	if (argc != 2) {
		printf("Invalid number of parameters\n");
		exit(0);
	}
	char stringPort[6];
	strcpy_s(stringPort, argv[1]);
	int port = atoi(stringPort);

	// Bind address to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, SERVER_ADDR, &serverAddr.sin_addr);
	if (bind(listenSock, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error %d: Cannot associate a local address with server socket", WSAGetLastError());
		return 0;
	}

	// Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error %d: Cannot place server socket in state LISTEN", WSAGetLastError());
		return 0;
	}
	printf("Server started!\n");


	while (1) {
		// Communicate with client
		sockaddr_in clientAddr;
		char buff[BUFF_SIZE], clientIP[INET_ADDRSTRLEN];
		int ret, clientAddrLen = sizeof(clientAddr), clientPort;
		SOCKET connSock;

		// Accept request
		connSock = accept(listenSock, (sockaddr *)& clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR) {
			printf("Error %d: Cannot permit incoming connection.\n", WSAGetLastError());
			return 0;
		}
		else {
			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);
			printf("Accept incoming connection from %s:%d\n", clientIP, clientPort);
		}

		string sBuff = ""; // send buffer
		string rBuff; // receiver buffer

		// Receive message from client
		while (1) {
			rBuff = "";
			ret = recv(connSock, buff, BUFF_SIZE, 0);

			string data = convertToString(buff, ret);
			rBuff = rBuff + data;

			if (ret == SOCKET_ERROR) {
				printf("Error %d: Cannot receive data.\n", WSAGetLastError());
				break;
			}
			else if (ret == 0) {
				printf("Client disconnects.\n");
				break;
			}
			else {
				printf("Receive from client [%s:%d] %s\n", clientIP, clientPort, data.c_str());
				sBuff = sBuff + rBuff;
				string delimiter = ENDING_DELIMITER;
				vector<string> arrayMessage = split(sBuff, delimiter);

				for (string mess : arrayMessage) {
					int result = findSum(mess);
					if (result != -1) {
						string data = "+" + to_string(result) + ENDING_DELIMITER;
						ret = send(connSock, data.c_str(), strlen(data.c_str()), 0);
						if (ret != -1) {
							printf("Send: +%d\n", result);
						}

					}
					else {

						string mess = "-Failed: String contains non-number character\r\n";
						ret = send(connSock, mess.c_str(), strlen("-Failed: String contains non-number character\r\n"), 0);
						if (ret != -1) {
							printf("Send: -Failed: String contains non-number character\n");
						}

					}
					if (ret == SOCKET_ERROR) {
						printf("Error %d: Cannot send data.\n", WSAGetLastError());
					}
				}
			}
		}
		closesocket(connSock);
	}

	closesocket(listenSock);

	// Terminate Winsock
	WSACleanup();

}