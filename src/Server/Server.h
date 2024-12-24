#ifndef SERVER_H
#define SERVER_H

#include <string>

class Server {
public:
    Server(int port);
    ~Server();
    void start();
    void stop();

private:
    int serverSocket;
    bool running;
    void handleClient(int clientSocket);
    std::string processRequest(const std::string &request);
    std::string handleLogin(const std::string& email, const std::string& password);
    std::string handleRegister(const std::string& email, const std::string& name, const std::string& password, bool is_male, bool is_teacher);
};

#endif