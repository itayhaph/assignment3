#include "StompProtocol.h"
#include "ConnectionHandler.h"
#include <string>
#include <iostream>
#include <random>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &connectionHandler) : connectionHandler(connectionHandler), receipt(0)
{
}

bool StompProtocol::processLogin(int host, string username, string password)
{

    string frame = "CONNECT \n";
    frame.append("accept-version:1.2").append("\n");
    frame.append("host:" + host).append("\n");
    frame.append("login:" + username).append("\n");
    frame.append("passcode:" + password).append("\n");

    char ch = '^@';
    connectionHandler.sendFrameAscii(frame, ch);
    return true;
};

void StompProtocol::processJoin(string chanel)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    // Use the distribution to cover the entire range of int
    std::uniform_int_distribution<> dis(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    // Generate and return a random integer

    int id = dis(gen);
    string frame = "SUBSCRIBE";
    frame.append("destination: " + chanel).append("\n");
    frame.append("id: " + id).append("\n");
    frame.append("receipt: " + receipt).append("\n");
    char ch = '^@';

    connectionHandler.sendFrameAscii(frame, ch);
    receipt++;
}
// void StompProtocol::processExit(string data);
// void StompProtocol::processReport(string data);
// void StompProtocol::processSummery(string data);
// void StompProtocol::processLogout(string data);