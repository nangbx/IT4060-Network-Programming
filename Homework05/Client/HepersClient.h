#pragma once
#include <string>

/**
* @function convertToString: covert char* to string
* @param source<char*>: A pointer to a input string
* @param size<int>: variable is equal to size of input string
*
* return: return about the string type data of char* input

*/
std::string convertToString(char* a, int size);

/**
* @function isNumber: Checks if a string contains all numbers.
* @param str<string>: string input
*
* return: 1 if string containing all numbers
0 if string does not contain all numbers
*/
bool isNumber(std::string str);


