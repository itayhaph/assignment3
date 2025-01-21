#pragma once

#include <string>
#include "ConnectionHandler.h"

using namespace std;

class StompProtocol
{
    private:
        ConnectionHandler &connectionHandler;
        int receipt;

    public:
        StompProtocol(ConnectionHandler &connectionHandler);
        bool processLogin(int host, string username, string password);
        void processJoin(string chanel);
        void processExit(string data);
        void processReport(string data);
        void processSummary(string data);
        bool processLogout();
};
