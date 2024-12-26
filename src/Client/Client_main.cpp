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
    std::string response;

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
            client.sendRequest(command);
        }
        else if (command == "LOGIN")
        {
            std::string email, password;
            std::cout << "Email: ";
            std::getline(std::cin, email);
            std::cout << "Password: ";
            std::getline(std::cin, password);

            command = "LOGIN/" + email + "/" + password;
            response = client.sendRequest(command);
            if (response == "200;Login successful by teacher")
            {
                // Hiển thị các chức năng khác sau khi đăng nhập thành công
                while (true)
                {
                    std::cout << "\nSelect an teacher's option:\n";
                    std::cout << "1. Update Profile\n";
                    std::cout << "2. Logout\n";
                    std::cout << "3. Exit\n";

                    int option;
                    std::cout << "Enter your choice: ";
                    std::cin >> option;
                    std::cin.ignore(); // Để bỏ qua newline sau khi nhập option

                    if (option == 1)
                    {
                        // Cập nhật thông tin người dùng (ví dụ)
                        std::cout << "Updating profile...\n";
                        // Thực hiện các thao tác cần thiết
                    }
                    else if (option == 2)
                    {
                        // Đăng xuất
                        command = "LOGOUT/";
                        client.sendRequest(command);
                        std::cout << "Logged out.\n";
                        break; // Thoát khỏi vòng lặp các chức năng sau khi logout
                    }
                    else if (option == 3)
                    {
                        command = "EXIT/";
                        client.sendRequest(command);
                        std::cout << "Exiting...\n";
                        return 0; // Thoát khỏi chương trình
                    }
                    else
                    {
                        std::cout << "Invalid option. Try again.\n";
                    }
                }
            }
            else if (response == "200;Login successful by student")
            {
                // Hiển thị các chức năng khác sau khi đăng nhập thành công
                while (true)
                {
                    std::cout << "\nSelect an student's option:\n";
                    std::cout << "1. Update Profile\n";
                    std::cout << "2. Logout\n";
                    std::cout << "3. Exit\n";

                    int option;
                    std::cout << "Enter your choice: ";
                    std::cin >> option;
                    std::cin.ignore(); // Để bỏ qua newline sau khi nhập option

                    if (option == 1)
                    {
                        // Cập nhật thông tin người dùng (ví dụ)
                        std::cout << "Updating profile...\n";
                        // Thực hiện các thao tác cần thiết
                    }
                    else if (option == 2)
                    {
                        // Đăng xuất
                        command = "LOGOUT/";
                        client.sendRequest(command);
                        std::cout << "Logged out.\n";
                        break; // Thoát khỏi vòng lặp các chức năng sau khi logout
                    }
                    else if (option == 3)
                    {
                        command = "EXIT/";
                        client.sendRequest(command);
                        std::cout << "Exiting...\n";
                    }
                    else
                    {
                        std::cout << "Invalid option. Try again.\n";
                    }
                }
            }
            else
            {
                continue;
            }
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
    }

    return 0;
}