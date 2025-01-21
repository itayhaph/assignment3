#include "StompProtocol.h"
#include "ConnectionHandler.h"
#include "Auxiliary.h"
#include <string>
#include <iostream>
#include <random>

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &connectionHandler) : connectionHandler(connectionHandler), receipt(0),gen(std::random_device{}()),
          dis(0, std::numeric_limits<int>::max())
{
}

void StompProtocol::processLogin(int host, string username, string password)
{
    string frame = "CONNECT \n";
    frame.append("accept-version:1.2").append("\n");
    frame.append("host:" + host).append("\n");
    frame.append("login:" + username).append("\n");
    frame.append("passcode:" + password).append("\n");

    // saving the username for the report command
    setUsername(username);

    connectionHandler.sendLine(frame);
};

void StompProtocol::processJoin(string chanel)
{
    int id = dis(gen);
    string frame = "SUBSCRIBE\n";
    frame.append("destination: " + chanel).append("\n");
    frame.append("id: " + id).append("\n");
    frame.append("receipt: " + receipt).append("\n");

    connectionHandler.sendLine(frame);
    receiptToCommand[receipt] = {"SUBSCRIBE", chanel};
    chanelToId[chanel] = id;
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
    auto it = chanelToId.find(chanel);
    if (it != chanelToId.end())
    {
        return it->second; // Return the found pair
    }
    return -1; // Return a default pair indicating "not found"
}

void StompProtocol::processExit(string chanel)
{
    int id = getId(chanel);
    string frame = "UNSUBSCRIBE\n";
    frame.append("id: " + id).append("\n");
    frame.append("receipt: " + receipt).append("\n");

    connectionHandler.sendLine(frame);
    receiptToCommand[receipt] = {"UNSUBSCRIBE", chanel};
    receipt++;
}

void StompProtocol::processReport(string filePath)
{
    names_and_events nne = parseEventsFile(filePath);
    std::vector<Event> sortedEvents = events;

    // Sorting the new vector
    std::sort(sortedEvents.begin(), sortedEvents.end(), [](const Event &a, const Event &b)
              {
                  if (a.get_date_time() == b.get_date_time())
                  {
                      return a.get_name() < b.get_name(); // Lexicographical order
                  }
                  return a.get_date_time() < b.get_date_time(); // Order by dateTime
              });

    for (Event &event : sortedEvents)
    {
        event.setEventOwnerUser(username);
        // command:
        string frame = "SEND\n";
        frame.append("destination:" + event.get_channel_name()).append("\n");

        // new line for body:
        frame.append("\n");
        frame.append("user: " + username).append("\n");
        frame.append("city: " + event.get_city()).append("\n");
        frame.append("event name: " + event.get_name()).append("\n");
        frame.append("date time: " + event.get_date_time()).append("\n");
        frame.append("general information:\n");
        frame.append("  active: ").append(event.get_general_information().find("active")->second);
        frame.append("  forces_arrival_at_scene: ").append(event.get_general_information().find("forces_arrival_at_scene")->second);
        frame.append("description:\n");
        frame.append(event.get_description()).append("\n");

        connectionHandler.sendLine(frame);
        events.push_back(event);
    }
};

void StompProtocol::processSummary(string channelName, string user, string filePath)
{
    Auxiliary aux;
    std::vector<Event> filteredEvents;
    std::ofstream outFile(filePath);

    if (!outFile)
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    int activeCount = 0;
    int forcesArrivalCount = 0;

    for (const auto &event : events)
    {
        // Count active = true
        auto activeIt = event.get_general_information().find("active");
        if (activeIt != event.get_general_information().end() && activeIt->second == "true")
        {
            ++activeCount;
        }

        // Count forces_arrival_at_scene = true
        auto forcesArrivalIt = event.get_general_information().find("forces_arrival_at_scene");
        if (forcesArrivalIt != event.get_general_information().end() && forcesArrivalIt->second == "true")
        {
            ++forcesArrivalCount;
        }

        // Check if the event has the specific channel and name
        if (event.get_channel_name() == channelName && event.getEventOwnerUser() == user)
        {
            filteredEvents.push_back(event);
        }
    }

    outFile << "Channel <" << channelName << ">\n";
    outFile << "Stats:\n";
    outFile << "Total: " << filteredEvents.size() << "\n";
    outFile << "active: " << activeCount << "\n";
    outFile << "forces arrival at scene:" << forcesArrivalCount << "\n";
    outFile << "Event Reports:\n";

    for (size_t i = 0; i < filteredEvents.size(); ++i)
    {
        string dateTime = aux.epoch_to_date(filteredEvents[i].get_date_time());
        string summary = filteredEvents[i].get_description();
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
    string frame = "DISCONNECT \n";
    frame.append("receipt:" + receipt);

    connectionHandler.sendLine(frame);
};

void StompProtocol::handleReceipt(string receiptId)
{
    // ready to terminate because the last message arrived
    pair<string, string> receiptPair = getReceipt(receipt);
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
        terminate();
        // isTerminated = true;
    }
}

void StompProtocol::terminate()
{
    receiptToCommand.clear();
    connectionHandler.close();
}

void StompProtocol::handleMessage(std::unordered_map<std::string, std::string> headers, string body)
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
        for (const auto &[key, value] : parsedBody)
        {
            if (key != "user" && key != "city" && key != "event name" &&
                key != "date time" && key != "description")
            {
                general_information[key] = value;
            }
        }

        // adding the event to the events vector
        events.push_back(Event(channel_name, city, name, date_time, description, general_information));
    }
}

// bool StompProtocol::shouldTerminate()
// {
//     return isTerminated;
// }