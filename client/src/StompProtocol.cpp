#include "StompProtocol.h"
#include "ConnectionHandler.h"
#include "Auxiliary.h"
#include <string>
#include <iostream>
#include <random>
#include <unordered_map>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &connectionHandler) : connectionHandler(connectionHandler), receipt(0),
                                                                     receiptToCommand(),
                                                                     channelToId(),
                                                                     events(),
                                                                     username("")
{
}

void StompProtocol::processLogin(std::string host, std::string username, std::string password)
{
    std::string frame = "CONNECT \n";
    frame.append("accept-version:1.2").append("\n");
    frame.append("host:" + host).append("\n");
    frame.append("login:" + username).append("\n");
    frame.append("passcode:" + password).append("\n");

    // saving the username for the report command
    setUsername(username);

    connectionHandler.sendLine(frame);
};

void StompProtocol::processJoin(std::string chanel)
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dis(1, std::numeric_limits<int>::max());

    int id = dis(gen);
    std::string frame = "SUBSCRIBE\n";
    frame.append("destination: " + chanel).append("\n");
    frame.append("id: " + std::to_string(id)).append("\n");
    frame.append("receipt: " + std::to_string(receipt)).append("\n");

    connectionHandler.sendLine(frame);
    receiptToCommand[receipt] = {"SUBSCRIBE", chanel};
    channelToId[chanel] = id;
    receipt++;
}

std::pair<std::string, std::string> StompProtocol::getReceipt(int receipt)
{
    auto it = receiptToCommand.find(receipt);
    if (it != receiptToCommand.end())
    {
        return it->second; // Return the found pair
    }
    return {"", ""}; // Return a default pair indicating "not found"
}

int StompProtocol::getId(string chanel)
{
    auto it = channelToId.find(chanel);
    if (it != channelToId.end())
    {
        return it->second; // Return the found pair
    }
    return -1; // Return a default pair indicating "not found"
}

void StompProtocol::processExit(string chanel)
{
    int id = getId(chanel);
    std::string frame = "UNSUBSCRIBE\n";
    frame.append("id: " + std::to_string(id)).append("\n");
    frame.append("receipt: " + std::to_string(receipt)).append("\n");

    connectionHandler.sendLine(frame);
    receiptToCommand[receipt] = {"UNSUBSCRIBE", chanel};
    receipt++;
}

void StompProtocol::processReport(string filePath)
{
    names_and_events nne = parseEventsFile(filePath);
    // Sorting the events by date time and then by name
    std::sort(nne.events.begin(), nne.events.end(), [](const Event &a, const Event &b)
              {
                  if (a.get_date_time() == b.get_date_time())
                  {
                      return a.get_name() < b.get_name(); // Lexicographical order
                  }
                  return a.get_date_time() < b.get_date_time(); // Order by dateTime
              });
   
    for (Event &event : nne.events)
    {
        event.setEventOwnerUser(username);
        // command:
        std::string frame = "SEND\n";
        frame.append("destination:" + event.get_channel_name()).append("\n");

        // new line for body:
        frame.append("\n");
        frame.append("user: " + username).append("\n");
        frame.append("city: " + event.get_city()).append("\n");
        frame.append("event name: " + event.get_name()).append("\n");
        frame.append("date time: " + std::to_string(event.get_date_time())).append("\n");
        frame.append("general information:\n");

        string active = event.get_general_information().at("active");
        string forces = event.get_general_information().at("forces_arrival_at_scene");

        frame.append("active: ").append(active).append("\n");
        frame.append("forces_arrival_at_scene: ").append(forces).append("\n");
        frame.append("description:\n");
        frame.append(event.get_description()).append("\n");
        connectionHandler.sendLine(frame);
        events.push_back(event);
    }
};

void StompProtocol::processSummary(string channelName, std::string user, std::string filePath)
{
    Auxiliary aux;
    std::vector<Event> filteredEvents;
    std::ofstream outFile(filePath);

    if (!outFile)
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    std::sort(events.begin(), events.end(), [](const Event &a, const Event &b)
              {
                  if (a.get_date_time() == b.get_date_time())
                  {
                      return a.get_name() < b.get_name(); // Lexicographical order
                  }
                  return a.get_date_time() < b.get_date_time(); // Order by dateTime
              });
   
    int activeCount = 0;
    int forcesArrivalCount = 0;

    for(const auto event:events) {
        if (event.get_channel_name() == channelName && event.getEventOwnerUser() == user)
        {
            filteredEvents.push_back(event);
        }
    }
    
    for (const auto event : filteredEvents)
    {
        // Count active = true
        if (event.get_general_information().at("active") == "true")
        {
            ++activeCount;
        }

        // Count forces_arrival_at_scene = true
        if (event.get_general_information().at("forces_arrival_at_scene") == "true")
        {
            ++forcesArrivalCount;
        }
    }

    outFile << "Channel: " << channelName << "\n";
    outFile << "Stats:\n";
    outFile << "Total: " << filteredEvents.size() << "\n";
    outFile << "active: " << activeCount << "\n";
    outFile << "forces arrival at scene:" << forcesArrivalCount << "\n";
    outFile << "Event Reports:\n";

    for (size_t i = 0; i < filteredEvents.size(); ++i)
    {
        std::string dateTime = aux.epoch_to_date(filteredEvents[i].get_date_time());
        std::string summary = filteredEvents[i].get_description();
        if (summary.length() > 27)
        {
            summary = summary.substr(0, 27) + "...";
        }

        outFile << "Report_" << (i + 1) << ":\n";
        outFile << "  city: " << filteredEvents[i].get_city() << "\n";
        outFile << "  date time: " << dateTime << "\n";
        outFile << "  event name: " << filteredEvents[i].get_name() << "\n";
        outFile << "  summary: " << summary << "\n\n";
    }

    outFile.close();
};

void StompProtocol::processLogout()
{
    std::string frame = "DISCONNECT \n";
    frame.append("receipt:" + std::to_string(receipt));
    receiptToCommand[receipt] = {"DISCONNECT",""};

    connectionHandler.sendLine(frame);
};

void StompProtocol::handleReceipt(std::string receiptId)
{
    // ready to terminate because the last message arrived
    pair<std::string, std::string> receiptPair = getReceipt(stoi(receiptId));

    if (receiptPair.first == "SUBSCRIBE")
    {
        std::cout << "Joined channel " + receiptPair.second << std::endl;
    }

    else if (receiptPair.first == "UNSUBSCRIBE")
    {
        std::cout << "Exited channel " + receiptPair.second << std::endl;
    }

    else if (receiptPair.first == "DISCONNECT")
    {
        std::cout << "terminating "<< std::endl;
        terminate();
    }
}

void StompProtocol::terminate()
{
    receiptToCommand.clear();
    connectionHandler.close();
}

void StompProtocol::handleMessage(std::unordered_map<std::string, std::string> headers, std::string body)
{
    Auxiliary aux;
    std::map<std::string, std::string> parsedBody = aux.parseFormattedString(body);

    // only process the message if its from another user
    std::string user = parsedBody.at("user");

    if (user != username)
    {
        std::string channel_name = headers.at("destination");
        std::string city = parsedBody.at("city");
        std::string name = parsedBody.at("event name");
        int date_time = std::stoi(parsedBody.at("date time"));
        std::string description = parsedBody.at("description");
        std::map<std::string, std::string> general_information;

        // Extract general information
        for (const auto &pair : parsedBody)
        {
            const auto &key = pair.first;
            const auto &value = pair.second;

            if (key != "user" && key != "city" && key != "event name" &&
                key != "date time" && key != "description")
            {
                general_information[key] = value;
            }
        }

        // adding the event to the events vector
        Event event(channel_name, city, name, date_time, description, general_information);
        event.setEventOwnerUser(user);
        events.push_back(event);
    }
}

void StompProtocol::setUsername(const std::string &username)
{
    this->username = username;
}