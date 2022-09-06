#pragma once
#include <string>


#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024
#define NUM_THREAD 1000

// Declare constants
const std::string CLIENT_LOGIN{ "USER" };
const std::string CLIENT_POST{ "POST" };
const std::string CLIENT_BYE{ "BYE" };
const std::string ENDING_DELIMITER{ "\r\n" };
const std::string SUCCESS_LOGIN{ "10" + ENDING_DELIMITER };
const std::string ACCOUNT_LOCKED{ "11" + ENDING_DELIMITER };
const std::string ACCOUNT_DOES_NOT_EXIST{ "12" + ENDING_DELIMITER };
const std::string LOGGED_ANOTHER_DEVICE{ "13" + ENDING_DELIMITER };
const std::string LOGGED{ "14" + ENDING_DELIMITER };
const std::string DATA_DOES_NOT_EXIST{ "98" + ENDING_DELIMITER };
const std::string UNKNOWN_MESSAGE{ "99" + ENDING_DELIMITER };
const std::string SUCCESS_POST{ "20" + ENDING_DELIMITER };
const std::string NOT_LOGGED_IN{ "21" + ENDING_DELIMITER };
const std::string SUCCESS_LOGOUT{ "30" + ENDING_DELIMITER };