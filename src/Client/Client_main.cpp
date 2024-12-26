#include "Client.h"
#include <iostream>
#include <string>
bool parseBoolean(const std::string &str)
{
    return str == "true" || str == "1" || str == "yes";
}
int main()
{
    std::string serverIP = "127.168.111.17"; // Hoặc địa chỉ IP khác nếu server không chạy trên cùng máy
    int port = 8079;

    Client client(serverIP, port);
    std::string command;

    while (true)
    {
        std::cout << "Enter command (REGISTER/LOGIN) or type EXIT to quit: ";
        std::getline(std::cin, command);

        if (command == "REGISTER")
        {
            std::string email, name, password, isMaleStr, isTeacherStr;
            std::cout << "Email: ";
            std::getline(std::cin, email);
            std::cout << "Name: ";
            std::getline(std::cin, name);
            std::cout << "Password: ";
            std::getline(std::cin, password);
            std::cout << "Is Male (true/false): ";
            std::getline(std::cin, isMaleStr);
            std::cout << "Is Teacher (true/false): ";
            std::getline(std::cin, isTeacherStr);

            bool isMale = parseBoolean(isMaleStr);
            bool isTeacher = parseBoolean(isTeacherStr);

            command = "REGISTER/" + email + "/" + name + "/" + password + "/" + (isMale ? "1" : "0") + "/" + (isTeacher ? "1" : "0");
        }
        else if (command == "LOGIN")
        {
            std::string email, password;
            std::cout << "Email: ";
            std::getline(std::cin, email);
            std::cout << "Password: ";
            std::getline(std::cin, password);

            command = "LOGIN/" + email + "/" + password;
        }
        else if (command == "EXIT")
        {
            command = "EXIT/";
            client.sendRequest(command);
            break;
        }
        else
        {
            std::cout << "Invalid command\n";
            continue;
        }

        client.sendRequest(command);
    }

    return 0;
}