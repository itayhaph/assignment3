#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <map>

#include "Auxiliary.h"

std::vector<std::string> Auxiliary::parseArguments(const std::string& line) {
    std::vector<std::string> arguments;
    std::istringstream stream(line);
    std::string argument;

    while (stream >> argument) {
        arguments.push_back(argument);
    }

    return arguments;
}

std::string Auxiliary::epoch_to_date(time_t epoch_time) {
    // Convert epoch time to a tm structure
    std::tm* timeinfo = std::localtime(&epoch_time);

    // Create a stringstream to format the date and time
    std::ostringstream oss;

    // Format: day/month/year hour:minute
    oss << std::setfill('0') 
        << std::setw(2) << timeinfo->tm_mday << "/"
        << std::setw(2) << (timeinfo->tm_mon + 1) << "/"
        << std::setw(2) << (timeinfo->tm_year % 100) << " "
        << std::setw(2) << timeinfo->tm_hour << ":"
        << std::setw(2) << timeinfo->tm_min;

    return oss.str();
}

std::map<std::string, std::string> Auxiliary::parseFormattedString(const std::string& input) {
    std::map<std::string, std::string> parsedData;
    std::istringstream stream(input);
    std::string line;

    // Parse each line
    while (std::getline(stream, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Handle special cases like "description:"
        if (line.back() == ':') {
            std::string key = line.substr(0, line.size() - 1); // Remove the colon
            std::string value;

            // Read the next lines until another key or end of stream
            while (std::getline(stream, line)) {
                if (line.empty()) break;
                if (line.find(":") != std::string::npos) { // Check if it's another key
                    stream.seekg(-line.size() - 1, std::ios_base::cur); // Move back the stream position
                    break;
                }
                if (!value.empty()) value += "\n"; // Preserve multiline structure
                value += line;
            }

            parsedData[key] = value;
        }
        // Handle standard key-value pairs (key: value)
        else {
            size_t delimiterPos = line.find(":");
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 2); // Skip ": "
                parsedData[key] = value;
            }
        }
    }

    return parsedData;
}