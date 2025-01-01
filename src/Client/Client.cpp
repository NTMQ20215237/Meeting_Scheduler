#include "Client.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <jsoncpp/json/json.h>

Client::Client(const std::string &serverIP, int port)
{
    connectToServer(serverIP, port);
}

Client::~Client()
{
    close(clientSocket);
}

void Client::connectToServer(const std::string &serverIP, int port)
{
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported\n";
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection failed\n";
        exit(EXIT_FAILURE);
    }
}

std::string Client::sendRequest(const std::string &requestType, const std::string &request, std::string &response)
{
    std::string fullRequest = requestType + "\n" + request;
    if (send(clientSocket, fullRequest.c_str(), fullRequest.size(), 0) < 0)
    {
        perror("Failed to send request");
        return "";
    }

    char buffer[1024] = {0};
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        response = std::string(buffer, bytesRead);
        return response;
    }
    else
    {
        std::cerr << "Failed to receive response from server" << std::endl;
        return "";
    }
}

void Client::registerUser(const std::string &email, const std::string &name, bool isMale, const std::string &password, bool isTeacher)
{
    Json::Value requestJson;
    requestJson["email"] = email;
    requestJson["name"] = name;
    requestJson['is']  = isMale;
    requestJson["password"] = password;
    requestJson["is_teacher"] = isTeacher;
    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("REGISTER", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        if (responseJson["success"].asBool())
        {
            std::cout << "User registered successfully." << std::endl;
        }
        else
        {
            std::cout << "Failed to register user." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::loginUser(const std::string &email, const std::string &password)
{
    Json::Value requestJson;
    requestJson["email"] = email;
    requestJson["password"] = password;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("LOGIN", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        if (responseJson["success"].asBool())
        {
            std::cout << "Login successful." << std::endl;
        }
        else
        {
            std::cout << "Failed to login." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::viewTeacherTimeSlots(int teacherId)
{
    Json::Value requestJson;
    requestJson["teacher_id"] = teacherId;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("VIEW_TIME_SLOTS", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        for (const auto &slot : responseJson["time_slots"])
        {
            std::cout << "Start Time: " << slot["start_time"].asString()
                      << ", End Time: " << slot["end_time"].asString() << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::createTimeSlot(int teacherId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting)
{
    Json::Value requestJson;
    requestJson["teacher_id"] = teacherId;
    requestJson["start_time"] = startTime;
    requestJson["end_time"] = endTime;
    requestJson["is_group_meeting"] = isGroupMeeting;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);
    std::string response;
    sendRequest("CREATE_TIME_SLOT", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        if (responseJson["success"].asBool())
        {
            std::cout << "Time slot created successfully." << std::endl;
        }
        else
        {
            std::cout << "Failed to create time slot." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::editTimeSlot(int slotId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting)
{
    Json::Value requestJson;
    requestJson["slot_id"] = slotId;
    requestJson["start_time"] = startTime;
    requestJson["end_time"] = endTime;
    requestJson["is_group_meeting"] = isGroupMeeting;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("EDIT_TIME_SLOT", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        if (responseJson["success"].asBool())
        {
            std::cout << "Time slot edited successfully." << std::endl;
        }
        else
        {
            std::cout << "Failed to edit time slot." << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::viewMeetingsByDate(const std::string &date)
{
    Json::Value requestJson;
    requestJson["date"] = date;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("VIEW_MEETINGS_BY_DATE", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        for (const auto &meeting : responseJson["meetings"])
        {
            std::cout << "Meeting ID: " << meeting["id"].asInt()
                      << ", Teacher ID: " << meeting["teacher_id"].asInt()
                      << ", Student ID: " << meeting["student_id"].asInt()
                      << ", Start Time: " << meeting["start_time"].asString()
                      << ", End Time: " << meeting["end_time"].asString()
                      << ", Is Group Meeting: " << meeting["is_group_meeting"].asBool() << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}

void Client::viewMeetingsByWeek(const std::string &startDate, const std::string &endDate)
{
    Json::Value requestJson;
    requestJson["start_date"] = startDate;
    requestJson["end_date"] = endDate;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("VIEW_MEETINGS_BY_WEEK", request, response);

    Json::Value responseJson;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    std::istringstream responseStream(response);

    if (Json::parseFromStream(readerBuilder, responseStream, &responseJson, &errors))
    {
        for (const auto &meeting : responseJson["meetings"])
        {
            std::cout << "Meeting ID: " << meeting["id"].asInt()
                      << ", Teacher ID: " << meeting["teacher_id"].asInt()
                      << ", Student ID: " << meeting["student_id"].asInt()
                      << ", Start Time: " << meeting["start_time"].asString()
                      << ", End Time: " << meeting["end_time"].asString()
                      << ", Is Group Meeting: " << meeting["is_group_meeting"].asBool() << std::endl;
        }
    }
    else
    {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
    }
}
