#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <map>
#include "event.h" // Assuming this is the provided event parser
#include "stomp_connection_handler.h" // Custom class to handle STOMP connection

std::mutex mutex;
bool isLoggedIn = false;
std::string activeUser;

// Function to process user commands
void handleUserInput(StompConnectionHandler& handler) {
    std::string command;
    while (true) {
        std::getline(std::cin, command);

        if (command.rfind("login", 0) == 0) {
            if (isLoggedIn) {
                std::cout << "The client is already logged in, log out before trying again" << std::endl;
                continue;
            }

            std::string host, username, password;
            size_t firstSpace = command.find(' ');
            size_t secondSpace = command.find(' ', firstSpace + 1);
            size_t thirdSpace = command.find(' ', secondSpace + 1);

            if (firstSpace == std::string::npos || secondSpace == std::string::npos || thirdSpace == std::string::npos) {
                std::cout << "Error: Invalid login command format" << std::endl;
                continue;
            }

            host = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            username = command.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
            password = command.substr(thirdSpace + 1);

            if (handler.connect(host)) {
                if (handler.sendConnectFrame(username, password)) {
                    isLoggedIn = true;
                    activeUser = username;
                    std::cout << "Login successful" << std::endl;
                }
            } else {
                std::cout << "Could not connect to server" << std::endl;
            }
        } else if (command.rfind("join", 0) == 0) {
            if (!isLoggedIn) {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = command.substr(5);
            if (handler.sendSubscribeFrame(channelName)) {
                std::cout << "Joined channel " << channelName << std::endl;
            }
        } else if (command.rfind("exit", 0) == 0) {
            if (!isLoggedIn) {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string channelName = command.substr(5);
            if (handler.sendUnsubscribeFrame(channelName)) {
                std::cout << "Exited channel " << channelName << std::endl;
            }
        } else if (command.rfind("report", 0) == 0) {
            if (!isLoggedIn) {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            std::string filePath = command.substr(7);
            auto events = parseEventsFile(filePath);

            for (const auto& event : events.events) {
                if (handler.sendEvent(event, events.channel_name, activeUser)) {
                    std::cout << "Reported event: " << event.event_name << std::endl;
                }
            }
        } else if (command.rfind("summary", 0) == 0) {
            if (!isLoggedIn) {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            size_t firstSpace = command.find(' ');
            size_t secondSpace = command.find(' ', firstSpace + 1);
            size_t thirdSpace = command.find(' ', secondSpace + 1);

            if (firstSpace == std::string::npos || secondSpace == std::string::npos || thirdSpace == std::string::npos) {
                std::cout << "Error: Invalid summary command format" << std::endl;
                continue;
            }

            std::string channelName = command.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            std::string user = command.substr(secondSpace + 1, thirdSpace - secondSpace - 1);
            std::string filePath = command.substr(thirdSpace + 1);

            handler.summarizeEvents(channelName, user, filePath);
        } else if (command == "logout") {
            if (!isLoggedIn) {
                std::cout << "Error: Client not logged in" << std::endl;
                continue;
            }

            if (handler.sendDisconnectFrame()) {
                isLoggedIn = false;
                activeUser.clear();
                std::cout << "Logout successful" << std::endl;
            }
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
}

// Function to handle server responses
void handleServerResponses(StompConnectionHandler& handler) {
    while (true) {
        std::string response = handler.receiveResponse();
        if (!response.empty()) {
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "Server: " << response << std::endl;
        }
    }
}

int main() {
    StompConnectionHandler handler;

    std::thread userInputThread(handleUserInput, std::ref(handler));
    std::thread serverResponseThread(handleServerResponses, std::ref(handler));

    userInputThread.join();
    serverResponseThread.join();

    return 0;
}
