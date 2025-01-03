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
#include <jsoncpp/json/json.h>

DatabaseManager::DatabaseManager(const std::string &connectionStr)
    : connectionString(connectionStr) {}

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
    else if (command == "CANCEL_MEETING")
    {
        std::string meetingID = parts[1];
        return handleCancelMeeting(clientSocket, meetingID);
    }
    else if (command == "SCHEDULE_INDIVIDUAL_MEETING")
    {
        std::string timeslot_id = parts[1];
        std::string student_id = parts[2];
        std::string type = parts[3];
        return handleScheduleIndividualMeeting(clientSocket, timeslot_id, student_id, type);
    }
    else if (command == "CHECK_MEETING_WITH_TEACHER")
    {
        std::string email = parts[1];
        int meetingId = std::stoi(parts[2]);
        return checkMeetingWithTeacher(email, meetingId);
    }
    else if (command == "CREATE_CONTENT")
    {
        int meetingId = std::stoi(parts[1]);
        std::string content = parts[2];
        return Server::handleCreateContent(meetingId, content);
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
    if (dbManager.loginUser(email, password) == 2)
    {
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            loggedInUsers[clientSocket] = email;
        }
        return "200;Login successful by teacher";
    }
    else if (dbManager.loginUser(email, password) == 1)
    {
        {
            std::lock_guard<std::mutex> lock(clientMutex);
            loggedInUsers[clientSocket] = email;
        }
        return "200;Login successful by student";
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

std::string Server::handleCancelMeeting(int clientSocket, const std::string &meetingID)
{
    std::string email;
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        auto it = loggedInUsers.find(clientSocket);
        if (it != loggedInUsers.end())
        {
            email = it->second;
        }
        else
        {
            return "401;Not logged in";
        }
    }

    if (dbManager.cancelMeeting(meetingID))
    {
        return "200;Meeting cancelled";
    }
    return "404;Meeting not found";
}

std::string Server::handleScheduleIndividualMeeting(int clientSocket, const std::string &timeslot_id, const std::string &student_id, const std::string &type)
{
    std::string email;
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        auto it = loggedInUsers.find(clientSocket);
        if (it != loggedInUsers.end())
        {
            email = it->second;
        }
        else
        {
            return "401;Not logged in";
        }
    }

    if (dbManager.scheduleIndividualMeeting(timeslot_id, student_id, type))
    {
        return "200;Meeting scheduled";
    }

    return "404;Meeting not found";
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

void Server::handleViewTimeSlotsRequest(const std::string &request, std::string &response)
{
    Json::Value requestJson;
    Json::Reader reader;
    reader.parse(request, requestJson);

    int teacherId = requestJson["teacher_id"].asInt();

    auto timeSlots = dbManager.getTeacherTimeSlots(teacherId);

    Json::Value responseJson;
    for (const auto &slot : timeSlots)
    {
        Json::Value slotJson;
        slotJson["start_time"] = slot.first;
        slotJson["end_time"] = slot.second;
        responseJson["time_slots"].append(slotJson);
    }

    Json::StreamWriterBuilder writer;
    response = Json::writeString(writer, responseJson);
}

void Server::handleCreateTimeSlotRequest(const std::string &request, std::string &response)
{
    Json::Value requestJson;
    Json::Reader reader;
    reader.parse(request, requestJson);

    int teacherId = requestJson["teacher_id"].asInt();
    std::string startTime = requestJson["start_time"].asString();
    std::string endTime = requestJson["end_time"].asString();
    bool isGroupMeeting = requestJson["is_group_meeting"].asBool();

    bool success = dbManager.createTimeSlot(teacherId, startTime, endTime, isGroupMeeting);

    Json::Value responseJson;
    responseJson["success"] = success;

    Json::StreamWriterBuilder writer;
    response = Json::writeString(writer, responseJson);
}

void Server::handleEditTimeSlotRequest(const std::string &request, std::string &response)
{
    Json::Value requestJson;
    Json::Reader reader;
    reader.parse(request, requestJson);

    int slotId = requestJson["slot_id"].asInt();
    std::string startTime = requestJson["start_time"].asString();
    std::string endTime = requestJson["end_time"].asString();
    bool isGroupMeeting = requestJson["is_group_meeting"].asBool();

    bool success = dbManager.editTimeSlot(slotId, startTime, endTime, isGroupMeeting);

    Json::Value responseJson;
    responseJson["success"] = success;

    Json::StreamWriterBuilder writer;
    response = Json::writeString(writer, responseJson);
}

void Server::handleViewMeetingsByDateRequest(const std::string &request, std::string &response)
{
    Json::Value requestJson;
    Json::Reader reader;
    reader.parse(request, requestJson);

    std::string date = requestJson["date"].asString();

    auto meetings = dbManager.getMeetingsByDate(date);

    Json::Value responseJson;
    for (const auto &meeting : meetings)
    {
        Json::Value meetingJson;
        meetingJson["id"] = std::get<0>(meeting);
        meetingJson["teacher_id"] = std::get<1>(meeting);
        meetingJson["student_id"] = std::get<2>(meeting);
        meetingJson["start_time"] = std::get<3>(meeting);
        meetingJson["end_time"] = std::get<4>(meeting);
        meetingJson["is_group_meeting"] = std::get<5>(meeting);
        responseJson["meetings"].append(meetingJson);
    }

    Json::StreamWriterBuilder writer;
    response = Json::writeString(writer, responseJson);
}

void Server::handleViewMeetingsByWeekRequest(const std::string &request, std::string &response)
{
    Json::Value requestJson;
    Json::Reader reader;
    reader.parse(request, requestJson);

    std::string startDate = requestJson["start_date"].asString();
    std::string endDate = requestJson["end_date"].asString();

    DatabaseManager dbManager(connectionString);
    auto meetings = dbManager.getMeetingsByWeek(startDate, endDate);

    Json::Value responseJson;
    for (const auto &meeting : meetings)
    {
        Json::Value meetingJson;
        meetingJson["id"] = std::get<0>(meeting);
        meetingJson["teacher_id"] = std::get<1>(meeting);
        meetingJson["student_id"] = std::get<2>(meeting);
        meetingJson["start_time"] = std::get<3>(meeting);
        meetingJson["end_time"] = std::get<4>(meeting);
        meetingJson["is_group_meeting"] = std::get<5>(meeting);
        responseJson["meetings"].append(meetingJson);
    }

    Json::StreamWriterBuilder writer;
    response = Json::writeString(writer, responseJson);
}
