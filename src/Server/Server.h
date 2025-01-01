#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>
#include <mutex>
#include <map>
#include "../Database/DatabaseManager.h"  // Include the DatabaseManager header file

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
    std::string connectionString;  // Add connection string as member
    std::mutex clientMutex;         // Mutex to protect logged-in users
    std::map<int, std::string> loggedInUsers; // Map to track logged-in users

    std::vector<std::string> split(const std::string &str, char delimiter);
    void handleClient(int clientSocket);
    std::string processRequest(int clientSocket, const std::string &request);                         // Updated signature
    std::string handleLogin(int clientSocket, const std::string &email, const std::string &password); // Updated signature
    std::string handleRegister(const std::string &email, const std::string &name, const std::string &password, bool is_male, bool is_teacher);
    std::string checkMeetingWithTeacher(const std::string &email, int meetingId);
    std::string handleCreateContent(int meetingId, const std::string &content);
    std::string handleLogout(int clientSocket); // Updated signature
    std::string handleCancelMeeting(int clientSocket, const std::string &meetingID); // Updated signature
    std::string handleScheduleIndividualMeeting(int clientSocket, const std::string &teacherEmail, const std::string &startAt, const std::string &title);
    void handleViewTimeSlotsRequest(const std::string &request, std::string &response);
    void handleCreateTimeSlotRequest(const std::string &request, std::string &response);
    void handleEditTimeSlotRequest(const std::string &request, std::string &response);
    void handleViewMeetingsByDateRequest(const std::string &request, std::string &response);
    void handleViewMeetingsByWeekRequest(const std::string &request, std::string &response);
};

#endif
