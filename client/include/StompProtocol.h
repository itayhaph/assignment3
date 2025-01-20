#pragma once

#include "../include/ConnectionHandler.h"

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
string command;

string data;
public:
processLogin(string data);
processJion(string data);
processExit(string data);
processReport(string data);
processSummery(string data);
processLogout(string data);
};
