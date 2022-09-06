#pragma once
#include <stdio.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <string.h>
#include <vector>
#include "Model.h"
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")


/**
* @function parseAccount: Change the string input to struct Account
*
* @param data<string> : string input;
*
* @return: Account struct if success
Exit the program if false
**/
Account parseAccount(std::string data);

/**
* @function getDataOnFile: Get data from file and convert to Account struct
*
* @param fileName<string>: The path to the file;
*
**/
void getDataOnFile(std::string fileName);

/**
* @function handleSendData: Send response data to clients.
*
* @param conn<SOCKET>: Socket needs to send data.
* @param data<stirng>: Data send.
*
**/
void handleSendData(SOCKET& conn, const std::string data);

/**
* @function handleLogin: Handle when server receives login request from client
*
* @param connectedSocket<SOCKET> : Socket receives request from client.
* @param body<string>: Data received from client.
* @param client<Account>: Account manages thread state.
*
**/
void handleLogin(SOCKET connectedSocket, std::string body, Account& client, int indexClient);

/**
* @function handlePost: Handle when server receives post request from client
*
* @param connectedSocket<SOCKET> : Socket receives request from client.
* @param body<string>: Data received from client.
* @param client<Account>: Account manages thread state.
*
**/
void handlePost(SOCKET connectedSocket, std::string body, Account client);

/**
* @function handleBye: Handle when server receives logout request from client
*
* @param connectedSocket<SOCKET> : Socket receives request from client.
* @param client<Account>: Account manages thread state.
*
**/
void handleBye(SOCKET connectedSocket, Account& client, int indexClient);

/**
* @function handleDisconnect: Handle when client disconnects
*
* @param client<Account>: Account manages thread state.
*
**/
void handleDisconnect(Account& client, int indexClient);

/**
* @function getInputServer: Handle data from command line argument
*
* @param argc<int> : Number of command line arguments.
* @param argv<char*>: Array containing command line arguments.
* @param port<int&>: A pointer to server port
*
**/
void getInputServer(int argc, char* argv[], int& port);