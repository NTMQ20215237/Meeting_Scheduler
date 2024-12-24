#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>

class MessageHandler {
public:
    std::string handleRequest(const std::string& request);
};

#endif
