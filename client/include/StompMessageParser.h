#ifndef STOMP_MESSAGE_PARSER_H
#define STOMP_MESSAGE_PARSER_H

#include <string>
#include <unordered_map>

class StompMessage {
private:
    std::string command;                                  // The STOMP command (e.g., CONNECT, MESSAGE, etc.)
    std::unordered_map<std::string, std::string> headers; // Headers as key-value pairs
    std::string body;                                     // The body of the message

public:
    // Setters (used internally by the parser)
    void setCommand(const std::string& cmd);
    void addHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& b);

    // Getters
    std::string getCommand() const;
    std::unordered_map<std::string, std::string> getHeaders() const;
    std::string getHeader(const std::string& key) const;
    std::string getBody() const;
};

class StompMessageParser {
public:
    // Parses a raw STOMP message and returns a StompMessage object
    static StompMessage parseMessage(const std::string& message);

private:
    // Helper function to trim whitespace from a string
    static std::string trim(const std::string& str);
};

#endif // STOMP_MESSAGE_PARSER_H
