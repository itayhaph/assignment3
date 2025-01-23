#pragma once

#include <string>
#include "ConnectionHandler.h"
#include "event.h"
#include <unordered_map>
#include <random>


using namespace std;

class StompProtocol
{
private:
    ConnectionHandler &connectionHandler;
    int receipt;
    //bool isTerminated;
    std::mt19937 gen; 
    std::uniform_int_distribution<> dis;
    std::unordered_map<int, std::pair<std::string, std::string>> receiptToCommand;
    std::unordered_map<std::string, int> channelToId;
    vector<Event> events;
    string username;

public:
    StompProtocol(ConnectionHandler &connectionHandler);
    void processLogin(string host, string username, string password);
    void processJoin(string chanel);
    void processExit(string data);
    void processReport(string filePath);
    void processSummary(string channelName, string user, string filePath);
    void processLogout();
    void handleReceipt(string data);
    void handleMessage(std::unordered_map<std::string, std::string> headers, string body);
    void terminate();
    // bool shouldTerminate();
    std::pair<std::string, std::string> getReceipt(int receipt);
    int getId(string chanel);
    void setUsername(const std::string& username);
};
