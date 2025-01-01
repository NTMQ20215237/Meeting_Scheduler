#ifndef CLIENT_H
#define CLIENT_H
#include <any>
#include <string>
#include <vector>
using namespace std;

class Client
{
public:
    Client(const std::string &serverIP, int port);
    ~Client();
    std::string sendRequest(const std::string &request);
    std::vector<std::string> split(const std::string &str, char delimiter);


private:
    int clientSocket;
    void connectToServer(const std::string &serverIP, int port);
};

#endif