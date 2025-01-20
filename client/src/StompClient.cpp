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

using std::vector;
using namespace std;

std::mutex mutex;
bool isLoggedIn = false;
std::string activeUser;


void handleUserInput(StompProtocol& protocol)
{
    std::string command;
    while (true)
    {
        
        Auxiliary aux;
        std::getline(std::cin, command);
        vector<string> line = aux.parseArguments(command);

        if (line[0] == "login")
        {
            if (isLoggedIn)
            {
                std::cout << "The client is already logged in" << std::endl;
                continue;
            }

            int host = std::stoi(line[1]);
            string username = line[2];
            string password = line[3];

            StompProtocol::protocol.processLogin(host,username,password);
        }
        else if (line[0] == "join")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = line[1];
            protocol.sendSubscribeFrame(channelName);
        }
        else if (line[0]=="exit")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = line[1];
            handler.sendUnsubscribeFrame(channelName);
            
        }
        else if (line[0]=="report")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string filePath = line[1];
            auto events = parseEventsFile(filePath);

            for (const auto &event : events.events)
            {
                handler.sendEvent(event, events.channel_name, activeUser);
            }
        }
        else if (line[0]=="summary")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = line[1];
            std::string user = line[2];
            std::string filePath = line[3];

            handler.summarizeEvents(channelName, user, filePath);
        }
        else if (line[0]=="logout")
        {
            if (!isLoggedIn)
            {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            if (handler.sendDisconnectFrame())
            {
                isLoggedIn = false;
            }
        }
       
    }
}

void handleServerResponses(StompProtocol& protocol)
{
    while (true)
    {
        std::string response = handler.receiveResponse();
        if (!response.empty())
        {
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "Server: " << response << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    
    ConnectionHandler handler = ConnectionHandler(argv[0],(short) argv[1]);
    hendler.connect();
    StompProtocol protocol=StompProtocol(handler);
    std::thread userInputThread(handleUserInput, std::ref(protocol));
    std::thread serverResponseThread(handleServerResponses, std::ref(protocol));

    userInputThread.join();
    serverResponseThread.join();

    return 0;
}
