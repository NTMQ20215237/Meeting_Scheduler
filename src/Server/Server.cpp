#include "Server.h"
#include "../Database/DatabaseManager.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <mutex>
#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

DatabaseManager dbManager("dbname=meeting_scheduler user=admin password=secret host=localhost");
std::mutex clientMutex;
std::map<int, std::string> loggedInUsers; // Map to track logged-in users

Server::Server(int port) : running(false)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 3) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

Server::~Server()
{
    stop();
}

void Server::start()
{
    running = true;
    std::cout << "Server started.\n";

    while (running)
    {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket >= 0)
        {
            std::thread(&Server::handleClient, this, clientSocket).detach();
        }
        else
        {
            perror("Accept");
        }
    }
}

void Server::stop()
{
    if (running)
    {
        close(serverSocket);
        running = false;
    }
}

void Server::handleClient(int clientSocket)
{
    char buffer[1024] = {0};
    while (true)
    {
        int bytesRead = read(clientSocket, buffer, 1024);
        if (bytesRead > 0)
        {
            std::string request(buffer, bytesRead);
            std::string response = processRequest(clientSocket, request);
            send(clientSocket, response.c_str(), response.size(), 0);
        }
    }
    close(clientSocket);
}

std::vector<std::string> Server::split(const std::string &str, char delimiter)
{
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        result.push_back(token);
    }
    return result;
}

std::string Server::processRequest(int clientSocket, const std::string &request)
{
    std::vector<std::string> parts = split(request, '/');

    if (parts.empty())
    {
        return "400;Invalid request format";
    }

    std::string command = parts[0];

    if (command == "LOGIN")
    {
        std::string email = parts[1];
        std::string password = parts[2];
        return handleLogin(clientSocket, email, password);
    }
    else if (command == "REGISTER")
    {
        std::string email = parts[1];
        std::string name = parts[2];
        std::string password = parts[3];
        int is_male = std::stoi(parts[4]);
        bool isMale = (is_male != 0); // Convert to bool
        int is_teacher = std::stoi(parts[5]);
        bool isTeacher = (is_teacher != 0); // Convert to bool
        return handleRegister(email, name, password, isMale, isTeacher);
    }
    else if (command == "CHECK_MEETING_WITH_TEACHER")
    {
        std::string email = parts[1];
        int meetingId = std::stoi(parts[2]);
        return checkMeetingWithTeacher(email, meetingId);
    }
    else if (command == "CREATE_CONTENT")
    {
        // Handle ENTER_CONTENT command
        // Example: ENTER_CONTENT/1/Meeting minutes content
        // Extract meeting ID and content from parts
        int meetingId = std::stoi(parts[1]);
        std::string content = parts[2];
        // Call a method to handle this command
        return Server::handleCreateContent(meetingId, content);
        // handleEnterContent(meetingId, content);
        return "200;Content entered successfully";
    }
    else if (command == "VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT")
    {
        // Handle VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT command
        // Example: VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT/teacher_email/student_name
        // Extract teacher email and student name from parts
        std::string teacherEmail = parts[1];
        std::string studentName = parts[2];
        // Call a method to handle this command
        return Server::handleViewMeetingDetailsAssociatingStudent(teacherEmail, studentName);
        // handleViewMeetingDetailsAssociatingStudent(teacherEmail, studentName);
    }
    else if (command == "LOGOUT")
    {
        return handleLogout(clientSocket);
    }
    else if (command == "EXIT")
    {
        close(clientSocket);
        return "200;Connection closed";
    }

    return "400;Unknown command";
}

std::string Server::handleRegister(const std::string &email, const std::string &name, const std::string &password, bool is_male, bool is_teacher)
{
    if (dbManager.registerUser(email, name, is_male, password, is_teacher))
    {
        return "201;Registration successful";
    }
    return "409;Email already registered or other error";
}

std::string Server::handleLogin(int clientSocket, const std::string &email, const std::string &password)
{
    std::vector<std::string> res = split(dbManager.loginUser(email, password), '/');
    if (res[0] == "2")
    {
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            loggedInUsers[clientSocket] = email;
        }
        return "200;Login successful by teacher/"+res[1];
    }
    else if (res[0] == "1")
    {
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            loggedInUsers[clientSocket] = email;
        }
        return "200;Login successful by student/"+res[1];
    }
    else
    {
        return "401;Invalid credentials";
    }
}

std::string Server::handleLogout(int clientSocket)
{
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        loggedInUsers.erase(clientSocket);
    }
    return "200;Logout successful";
}
std::string Server::checkMeetingWithTeacher(const std::string &email, int meetingId)
{
    if (dbManager.checkMeetingWithTeacher(email, meetingId))
    {
        return "200;Teacher and meeting match";
    }
    return "409;Conflict";
}
std::string Server::handleCreateContent(int meetingId, const std::string &content)
{
    if (dbManager.createContent(meetingId, content))
    {
        return "200;Content created successfully";
    }
    return "404;Bad request";
}
std::string Server::handleViewMeetingDetailsAssociatingStudent(const std::string &teacherEmail, const std::string &studentName)
{
    // Handle VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT command
    // Example: VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT/teacher_email/student_name
    // Call a method to handle this command
    std::string result = dbManager.viewMeetingDetailsAssociatingStudent(teacherEmail, studentName);
    if (result.empty())
    {
        return "404;No meeting details found";
    }
    else
    {
        return result;
    }
}