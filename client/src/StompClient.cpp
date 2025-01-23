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

using std::vector;
using namespace std;

bool isLoggedIn = false;
std::string activeUser;

void handleUserInput(StompProtocol &protocol, ConnectionHandler &handler)
{
    std::string command;
    // TODO figure out where we close this and the other while (close the threads)
    while (true)
    {
        Auxiliary aux;
        std::getline(std::cin, command);
        vector<string> line = aux.parseArguments(command);

        if (line[0] == "login")
        {
            if (!handler.connect())
            {
                std::cout << "The client is already logged in" << std::endl;
                continue;
            }
            else
            {
                string host = handler.getHost();
                string username = line[2];
                string password = line[3];

                handler.addCallback([&]()
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
                std::string channelName = line[1];
                handler.addCallback([&]()
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
                handler.addCallback([&]()
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

            handler.addCallback([&]()
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

            handler.addCallback([&]()
                                { protocol.processSummary(channelName, user, filePath); });
        }
        else if (line[0] == "logout")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            handler.addCallback([&]()
                                { protocol.processLogout(); });
        }
        else
        {
            std::cout << "Error: Client not logged in" << std::endl;
            continue;
        }
    }
}

void handleServerResponses(StompProtocol &protocol, ConnectionHandler &handler)
{
    while (true)
    {
        std::string line = "";
        handler.processNextCallback();

        if (handler.hasDataToRead())
        {
            bool response = handler.getLine(line);
            std::cout << line << std::endl;

            

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

    std::cout << "connect to " << argv[1] << "#####" << argv[2] << std::endl;
    std::string host = argv[1];
    short port = static_cast<short>(std::stoi(argv[2]));
    std::queue<std::function<void()>> callbackQueue;
    ConnectionHandler handler(host, port, callbackQueue);
    StompProtocol protocol = StompProtocol(handler);
    std::thread userInputThread(handleUserInput, std::ref(protocol), std::ref(handler));
    std::thread serverResponseThread(handleServerResponses, std::ref(protocol), std::ref(handler));

    userInputThread.join();
    serverResponseThread.join();

    return 0;
}
