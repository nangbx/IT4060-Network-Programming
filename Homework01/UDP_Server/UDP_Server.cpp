#include <stdio.h>
#include <WINSOCK2.h>
#include <WS2tcpip.h>
#include <string>
#include <string.h>
#pragma comment (lib, "Ws2_32.lib")

#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 64000


using namespace std;



int main(int argc, char* argv[]) {

	// Initiate WinSock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);

	if (WSAStartup(wVersion, &wsaData)) {
		printf("Winsock 2.2 is not supported\n");
		return 0;
	}

	// Construct socket
	SOCKET server;
	server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (server == INVALID_SOCKET) {
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

	if (bind(server, (sockaddr *)&serverAddr, sizeof(serverAddr))) {

		printf("Error %d: Cannot bind this address.", WSAGetLastError());
		return 0;
	}

	printf("Server started!\n");

	// Communicate with client
	sockaddr_in clientAddr;
	char buff[BUFF_SIZE], clientIP[INET_ADDRSTRLEN];
	int ret, clientAddrLen = sizeof(clientAddr), clientPort;
	addrinfo* result;
	int rc;
	sockaddr_in* address;
	addrinfo hints;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_family = AF_INET;
	char ipStr[INET_ADDRSTRLEN];

	while (1) {
		// Receive message
		ret = recvfrom(server, buff, BUFF_SIZE, 0, (sockaddr *)&clientAddr, &clientAddrLen);
		if (ret == SOCKET_ERROR) {
			printf("Error %d: Cannot receive data\n", WSAGetLastError());
		}
		else if (strlen(buff) > 0) {
			buff[ret] = 0;

			inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, sizeof(clientIP));
			clientPort = ntohs(clientAddr.sin_port);

			printf("Receive from client [%s:%d] %s\n", clientIP, clientPort, buff);
			// Domain name resolution
			rc = getaddrinfo(buff, NULL, &hints, &result);
			if (rc == 0) {
				while (result) {
					address = (struct sockaddr_in*) result->ai_addr;
					inet_ntop(AF_INET, &address->sin_addr, ipStr, sizeof(ipStr));
					string data(ipStr);
					data = '+' + data;
					ret = sendto(server, data.c_str(), strlen(data.c_str()), 0, (SOCKADDR *)&clientAddr, sizeof(clientAddr));
					if (ret == SOCKET_ERROR) {
						printf("Error %d: Cannot send data\n", WSAGetLastError());
						break;
					}
					else {
						printf("Send data: %s\n", data.c_str());
					}
					result = result->ai_next;
				}
				ret = sendto(server, "END", strlen("END"), 0, (SOCKADDR *)&clientAddr, sizeof(clientAddr));
			}
			else {
				ret = sendto(server, "-Not found information", strlen("-Not found information"), 0, (SOCKADDR *)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error %d: Cannot send data\n", WSAGetLastError());
				}
				else {
					printf("Send data: -Not found information\n");
				}
			}
			

		}
	}

	// Close socket
	closesocket(server);

	// Terminate WinSock
	WSACleanup();
	return 0;
}