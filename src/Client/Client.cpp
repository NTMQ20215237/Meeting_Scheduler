#include "Client.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <json/json.h>

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
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address/ Address not supported\n";
        return;
    }

    if (connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "Connection Failed\n";
        return;
    }
}

std::string Client::sendRequest(const std::string &request)
{
    // Gửi yêu cầu tới server
    send(clientSocket, request.c_str(), request.size(), 0);

    char buffer[1024] = {0};
    int bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead > 0)
    {
        std::string response(buffer, bytesRead);
        std::cout << "Server response: " << response << std::endl;
        return response; // Trả về phản hồi dưới dạng chuỗi
    }
    else
    {
        std::cerr << "Failed to receive response from server" << std::endl;
        return ""; // Trả về chuỗi rỗng nếu không nhận được phản hồi
    }
}

// Request và Xem lịch rảnh của teacher
void Client::viewTeacherTimeSlots(int teacherId)
{
    Json::Value requestJson;
    requestJson["teacher_id"] = teacherId;

    Json::StreamWriterBuilder writer;
    std::string request = Json::writeString(writer, requestJson);

    std::string response;
    sendRequest("VIEW_TIME_SLOTS", request, response);

    Json::Value responseJson;
    Json::Reader reader;
    reader.parse(response, responseJson);

    for (const auto &slot : responseJson["time_slots"])
    {
        std::cout << "Start Time: " << slot["start_time"].asString()
 
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
    Json::Reader reader;
    reader.parse(response, responseJson);

    if (responseJson["success"].asBool())
    {
        std::cout << "Time slot created successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to create time slot." << std::endl;
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
    Json::Reader reader;
    reader.parse(response, responseJson);

    if (responseJson["success"].asBool())
    {
        std::cout << "Time slot edited successfully." << std::endl;
    }
    else
    {
        std::cout << "Failed to edit time slot." << std::endl;
    }
}