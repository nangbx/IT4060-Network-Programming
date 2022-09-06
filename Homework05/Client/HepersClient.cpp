#include "HepersClient.h"

std::string convertToString(char* a, int size) {
	std::string s = "";
	for (int i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}

bool isNumber(std::string str) {
	for (char c : str) {
		if (!isdigit(c)) {
			return false;
		}
	}
	return true;
}


