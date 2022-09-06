#include "HepersClient.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include "Constant.h"
#include "ClientMessage.h"

#pragma comment(lib, "Ws2_32.lib")
void notiToUser(std::string mess)
{
	int pos = mess.find("\r\n");
	if (pos == std::string::npos)
	{
		printf("Message does not exist\n");
		return;
	}
	else
	{
		mess = mess.substr(0, pos);
	}
	if (mess == "10")
	{
		printf("Login successful\n");
	}
	else if (mess == "11")
	{
		printf("Account has been locked\n");
	}
	else if (mess == "12")
	{
		printf("Account does not exist\n");
	}
	else if (mess == "13")
	{
		printf("Account is logged in on another device\n");
	}
	else if (mess == "14")
	{
		printf("You are logged in\n");
	}
	else if (mess == "20")
	{
		printf("Post successfully\n");
	}
	else if (mess == "21")
	{
		printf("You have not signed in\n");
	}
	else if (mess == "30")
	{
		printf("Sign out successful\n");
	}
	else if (mess == "99")
	{
		printf("Message does not exist\n");
	}
	else
	{
		printf("%s\n", mess.c_str());
	}
}

std::string handleSendData(std::string mess, SOCKET &client, char *buff)
{
	// Add ending delimiter to message
	int ret;

	// Send message
	do
	{
		std::string sendMess = "";
		if (mess.length() < BUFF_SIZE)
		{ // When length of mess < BUFF_SIZE
			sendMess = mess;
		}
		else
		{ // When length of mess > BUFF_SIZE
			sendMess = mess.substr(0, BUFF_SIZE);
		}
		ret = send(client, sendMess.c_str(), strlen(sendMess.c_str()), 0);
		mess.erase(0, sendMess.length());
	} while (mess.length() != 0);

	if (ret == SOCKET_ERROR)
	{
		printf("Error %d: Cannot send data.\n", WSAGetLastError());
	}

	// Receive echo message
	std::string rBuff = "";
	while (rBuff.find(ENDING_DELIMITER) == std::string::npos)
	{
		ret = recv(client, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR)
		{
			printf("Error %d: Cannot receive data.\n", WSAGetLastError());
			return "";
		}
		rBuff = rBuff + convertToString(buff, ret);
	}

	return rBuff;
}
void handleLogin(SOCKET &client, char *buff)
{
	std::cin.sync();
	std::string username;
	printf("\n\n<---Login--->\n");
	printf("Enter your username: ");
	getline(std::cin, username);
	std::string serverMessage = handleSendData(LoginMessage(username), client, buff);
	notiToUser(serverMessage);
}

void handlePost(SOCKET &client, char *buff)
{
	std::cin.sync();
	std::string artical;
	printf("\n<---Post--->\n");
	printf("Enter your artical: ");
	getline(std::cin, artical);
	std::string serverMessage = handleSendData(PostsMessage(artical), client, buff);
	notiToUser(serverMessage);
}
void handleBye(SOCKET &client, char *buff)
{
	std::string serverMessage = handleSendData(ByeMessage(), client, buff);
	notiToUser(serverMessage);
}

void getInputClient(int argc, char *argv[], int &port, std::string &serverAddr)
{
	if (argc != 3)
	{
		printf("Invalid number of parameters\n");
		exit(0);
	}
	std::string stringPort;
	serverAddr = convertToString(argv[1], strlen(argv[1]));
	stringPort = convertToString(argv[2], strlen(argv[2]));
	if (!isNumber(stringPort))
	{
		printf("Invalid number of parameters\n");
		exit(0);
	}
	port = atoi(stringPort.c_str());
}