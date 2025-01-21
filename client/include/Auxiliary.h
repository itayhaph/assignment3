#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

class Auxiliary{
    public:
        static std::vector<std::string> parseArguments(const std::string& line);
        static std::string epoch_to_date(time_t epoch_time);
        static std::map<std::string, std::string> parseFormattedString(const std::string& frame);
};
