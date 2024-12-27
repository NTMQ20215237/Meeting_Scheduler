#include "Client.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

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
        std::cout << "Server response:\n " << response << std::endl;
        return response; // Trả về phản hồi dưới dạng chuỗi
    }
    else
    {
        std::cerr << "Failed to receive response from server" << std::endl;
        return ""; // Trả về chuỗi rỗng nếu không nhận được phản hồi
    }
}