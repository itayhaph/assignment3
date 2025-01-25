#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include <vector>
#include "Auxiliary.h"
#include "event.h"
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include "StompMessageParser.h"
#include <random>

using std::vector;
using namespace std;

bool isLoggedIn = false;

void handleServerResponses(StompProtocol &protocol, ConnectionHandler &handler)
{
    while (true)
    {
        std::string line = "";
        handler.processNextCallback();

        if (handler.hasDataToRead())
        {
            bool response = handler.getLine(line);

            if (response)
            {
                StompMessage frame = StompMessageParser::parseMessage(line);
                if (frame.getCommand() == "CONNECTED")
                {
                    isLoggedIn = true;
                    std::cout << "Login successful" << std::endl;
                }

                else if (frame.getCommand() == "RECEIPT")
                {
                    protocol.handleReceipt(frame.getHeader("receipt-id"));
                }
                else if (frame.getCommand() == "ERROR")
                {
                    std::cout << "Server Returned Error: " + frame.getHeader("message") << std::endl;

                    std::cout << frame.getHeader("receipt-id") << std::endl;

                    if (frame.getBody().length() > 0)
                    {
                        std::cout << frame.getBody() << std::endl;
                    }
                    isLoggedIn = false;
                    protocol.terminate();
                }
                else if (frame.getCommand() == "MESSAGE")
                {
                    protocol.handleMessage(frame.getHeaders(), frame.getBody());
                }
            }
        }
    }
}

int main(int argc, char **argv)
{
    std::string host;
    short port;

    if (argc == 3)
    {
        host = argv[1];
        port = static_cast<short>(std::stoi(argv[2]));
    }
    else
    {
        host = "stomp.cs.bgu.ac.il";
        port = static_cast<short>(std::stoi(argv[1]));
    }
    std::queue<std::function<void()>> callbackQueue;
    ConnectionHandler handler(host, port, callbackQueue);
    StompProtocol protocol = StompProtocol(handler);
    std::string command;

    std::thread serverResponseThread(handleServerResponses, std::ref(protocol), std::ref(handler));

    while (true)
    {
        Auxiliary aux;
        std::getline(std::cin, command);
        vector<string> line = aux.parseArguments(command);

        if (line[0] == "login")
        {
            if (isLoggedIn)
            {
                continue;
            }
            else
            {
                handler.connect();
                string host = line[1];
                string username = line[2];
                string password = line[3];

                handler.addCallback([host, username, password, &protocol]()
                                    { protocol.processLogin(host, username, password); });
            }
        }
        else if (line[0] == "join")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }
            else
            {
                string channelName = line[1];
                handler.addCallback([channelName, &protocol]()
                                    { protocol.processJoin(channelName); });
            }
        }
        else if (line[0] == "exit")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }
            else
            {
                std::string channelName = line[1];
                handler.addCallback([channelName, &protocol]()
                                    { protocol.processExit(channelName); });
            }
        }
        else if (line[0] == "report")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string filePath = line[1];

            handler.addCallback([filePath, &protocol]()
                                { protocol.processReport(filePath); });
        }
        else if (line[0] == "summary")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = line[1];
            std::string user = line[2];
            std::string filePath = line[3];

            handler.addCallback([channelName, user, filePath, &protocol]()
                                { protocol.processSummary(channelName, user, filePath); });
        }
        else if (line[0] == "logout")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            handler.addCallback([&protocol]()
                                { protocol.processLogout(); });
            isLoggedIn = false;
        }
        else
        {
            std::cout << "Error: Client not logged in" << std::endl;
            continue;
        }
    }

    return 0;
}
