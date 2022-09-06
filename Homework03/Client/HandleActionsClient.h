#pragma once
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

/**
* @function notiToUser: Display the message to the user.
* @param mess<string>: string input
*
*/
void notiToUser(std::string mess);

/**
* @function handleSendData: Send data to server and return received data.
* @param mess<string>: Data to send.
* @param client<SOCKET>: Socket connected to server.
* @param buff<char*>: Contains received data.
* return: string received from server

*/
std::string handleSendData(std::string mess, SOCKET &client, char* buff);

/**
* @function handleLogin: Handle when the user selects the login function
* @param client<SOCKET>: Socket connected to server.
* @param buff<char*>: Contains received data.

*/
void handleLogin(SOCKET& client, char* buff);

/**
* @function handlePost: Handle when the user selects the post function
* @param client<SOCKET>: Socket connected to server.
* @param buff<char*>: Contains received data.

*/
void handlePost(SOCKET& client, char* buff);

/**
* @function handleBye: Handle when the user selects the bye function
* @param client<SOCKET>: Socket connected to server.
* @param buff<char*>: Contains received data.

*/
void handleBye(SOCKET& client, char* buff);

/**
* @function getInputClient: Handle data from command line argument
* @param argc<int> : Number of command line arguments.
* @param argv<char*>: Array containing command line arguments.
* @param port<int&>: A pointer to server port
* @param serverAddr<string>: address server

*/
void getInputClient(int argc, char* argv[], int& port, std::string& serverAddr);