#ifndef CLIENT_H
#define CLIENT_H

#include <string>
using namespace std;

class Client
{
public:
    Client(const std::string &serverIP, int port);
    ~Client();
    void sendRequest(const std::string &request);

private:
    int clientSocket;
    void connectToServer(const std::string &serverIP, int port);
};

#endif