#include "HepersServer.h"
#include <vector>
#include <string>


std::vector<std::string> splitString(std::string str, std::string delimiter) {
	std::vector<std::string> out;
	int pos = 0;
	while ((pos = str.find(delimiter)) != std::string::npos) {
		out.push_back(str.substr(0, pos));
		str.erase(0, pos + delimiter.length());
	}
	out.push_back(str);
	return out;
}
std::vector<std::string> handleData(std::string& sBuff, std::string delimiter) {
	std::vector<std::string> out;
	int pos = 0;
	while ((pos = sBuff.find(delimiter)) != std::string::npos) {
		out.push_back(sBuff.substr(0, pos));
		sBuff.erase(0, pos + delimiter.length());
	}
	return out;
}
bool isNumber(std::string str) {
	for (char c : str) {
		if (!isdigit(c)) {
			return false;
		}
	}
	return true;
}
std::string convertToString(char* arr, int size) {
	std::string s = "";
	for (int i = 0; i < size; i++) {
		s = s + arr[i];
	}
	return s;
}
