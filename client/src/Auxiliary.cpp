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

std::string epoch_to_date(time_t epoch_time) {
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

std::map<std::string, std::string> parseFormattedString(std::string& input) {
    std::map<std::string, std::string> parsedData;
    std::istringstream stream(input);
    std::string line, currentKey, currentValue;

    while (std::getline(stream, line)) {
        // Remove leading and trailing spaces (optional)
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Ignore empty lines
        if (line.empty()) continue;

        // Check if line contains a "key : value" format
        size_t delimiterPos = line.find(" : ");
        if (delimiterPos != std::string::npos) {
            // Save previous key-value pair if it's multi-line
            if (!currentKey.empty() && !currentValue.empty()) {
                parsedData[currentKey] = currentValue;
                currentKey.clear();
                currentValue.clear();
            }

            // Extract key and value
            currentKey = line.substr(0, delimiterPos);
            currentValue = line.substr(delimiterPos + 3); // Skip " : "
        } else if (!currentKey.empty()) {
            // Handle multi-line value
            if (!currentValue.empty()) currentValue += "\n";
            currentValue += line;
        }
    }

    // Save the last key-value pair
    if (!currentKey.empty() && !currentValue.empty()) {
        parsedData[currentKey] = currentValue;
    }

    return parsedData;
}
