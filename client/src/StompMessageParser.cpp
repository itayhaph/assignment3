#include "StompMessageParser.h"
#include <sstream>

// --- StompMessage Implementation ---

void StompMessage::setCommand(const std::string& cmd) {
    command = cmd;
}

void StompMessage::addHeader(const std::string& key, const std::string& value) {
    headers[key] = value;
}

void StompMessage::setBody(const std::string& b) {
    body = b;
}

std::string StompMessage::getCommand() const {
    return command;
}

std::unordered_map<std::string, std::string> StompMessage::getHeaders() const {
    return headers;
}

std::string StompMessage::getHeader(const std::string& key) const {
    auto it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    return ""; // Return an empty string if the header does not exist
}

std::string StompMessage::getBody() const {
    return body;
}

// --- StompMessageParser Implementation ---

StompMessage StompMessageParser::parseMessage(const std::string& message) {
    StompMessage stompMessage;
    std::istringstream stream(message);
    std::string line;

    // Step 1: Parse the command (first line)
    if (std::getline(stream, line)) {
        stompMessage.setCommand(trim(line));
    }

    // Step 2: Parse headers (lines until an empty line is encountered)
    while (std::getline(stream, line) && !line.empty()) {
        size_t colonIndex = line.find(':');
        if (colonIndex != std::string::npos) {
            std::string key = trim(line.substr(0, colonIndex));
            std::string value = trim(line.substr(colonIndex + 1));
            stompMessage.addHeader(key, value);
        }
    }

    // Step 3: Parse the body (remaining lines)
    std::ostringstream bodyStream;

    while (std::getline(stream, line, '\0')) { // Null character terminates the body
        bodyStream << line;
    }
    
    stompMessage.setBody(trim(bodyStream.str()));

    return stompMessage;
}

// Helper function to trim whitespace from a string
std::string StompMessageParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == std::string::npos || last == std::string::npos) {
        return ""; // All whitespace
    }
    return str.substr(first, (last - first + 1));
}
