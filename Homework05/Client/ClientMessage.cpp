#include "ClientMessage.h"
#include "Constant.h"

std::string LoginMessage(std::string username) {
	return LOGIN + " " + username + ENDING_DELIMITER;
}
std::string PostsMessage(std::string article) {
	return POST + " " + article + ENDING_DELIMITER;
}
std::string ByeMessage() {
	return BYE + ENDING_DELIMITER;
}

