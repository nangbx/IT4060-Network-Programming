#include <vector>
#include <string>
#pragma once

/**
* @function split: split string by character delimiter
* @param sBuff<string>: string to split
* @param delimiter<string>: string delimiter
*
* return: vector substrings after being split

*/
std::vector<std::string> splitString(std::string str, std::string delimiter);

/**
* @function handleData: Handle data received from clients.
* @param sBuff<string>: Data to be processed.
* @param delimiter<string>: string delimiter
*
* return: vector substrings after handle

*/
std::vector<std::string> handleData(std::string& sBuff, std::string delimiter);

/**
* @function isNumber: Checks if a string contains all numbers.
* @param str<string>: string input
*
* return: 1 if string containing all numbers
0 if string does not contain all numbers
*/
bool isNumber(std::string str);

/**
* @function convertToString: covert char* to string
* @param source<char*>: A pointer to a input string
* @param size<int>: variable is equal to size of input string
*
* return: return about the string type data of char* input

*/
std::string convertToString(char* arr, int size);

