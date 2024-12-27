#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>

class Server
{
public:
    Server(int port);
    ~Server();
    void start();
    void stop();

private:
    int serverSocket;
    bool running;
    std::vector<std::string> split(const std::string &str, char delimiter);
    void handleClient(int clientSocket);
    std::string processRequest(int clientSocket, const std::string &request);                         // Updated signature
    std::string handleLogin(int clientSocket, const std::string &email, const std::string &password); // Updated signature
    std::string handleRegister(const std::string &email, const std::string &name, const std::string &password, bool is_male, bool is_teacher);
    std::string checkMeetingWithTeacher(const std::string &email, int meetingId);
    std::string handleCreateContent(int meetingId, const std::string &content);
    std::string handleViewMeetingDetailsAssociatingStudent(const std::string &email, const std::string &studentName);
    std::string handleLogout(int clientSocket); // Updated signature
};

#endif