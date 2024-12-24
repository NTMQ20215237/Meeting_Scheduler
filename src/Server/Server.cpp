#include "Server.h"
#include "../Database/DatabaseManager.h"
#include <iostream>
#include <thread>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
DatabaseManager dbManager("dbname=meeting_scheduler user=admin password=secret host=localhost");
Server::Server(int port) : running(false) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(serverSocket, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }
}

Server::~Server() {
    stop();
}

void Server::start() {
    running = true;
    std::cout << "Server started.\n";

    while (running) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket >= 0) {
            std::thread(&Server::handleClient, this, clientSocket).detach();
        } else {
            perror("Accept");
        }
    }
}

void Server::stop() {
    if (running) {
        close(serverSocket);
        running = false;
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[1024] = {0};
    int bytesRead = read(clientSocket, buffer, 1024);
    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);
        std::string response = processRequest(request);
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
}

std::string Server::processRequest(const std::string &request) {
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "LOGIN") {
        std::string email, password;
        iss >> email >> password;
        return handleLogin(email, password);
    } else if (command == "REGISTER") {
        std::string email, name, password;
        int is_male, is_teacher; // Using int to parse from '0'/'1' string representation
        iss >> email >> name >> password >> is_male >> is_teacher;
        return handleRegister(email, name, password, is_male != 0, is_teacher != 0);
    }
    return "400;Unknown command";
}

std::string Server::handleRegister(const std::string& email, const std::string& name, const std::string& password, bool is_male, bool is_teacher) {
    if (dbManager.registerUser(email, name, is_male, password, is_teacher)) {
        return "201;Registration successful";
    }
    return "409;Email already registered or other error";
}


std::string Server::handleLogin(const std::string& email, const std::string& password) {
    if (dbManager.loginUser(email, password)) {
        return "200;Login successful";
    }
    return "401;Invalid credentials";
}