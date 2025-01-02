#include "Client.h"
#include <iostream>
#include <string>
#include<vector>
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
    std::string token;

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
            std::vector<std::string> res = client.split(response, '/');

            if (res[0] == "200;Login successful by teacher")
            {
                token = res[1];
                std::cout << "Login successful by teacher\n";
                // Hiển thị các chức năng khác sau khi đăng nhập thành công
                while (true)
                {
                    std::cout << "\nSelect an teacher's option:\n";
                    std::cout << "1. Enter content (meeting minutes) for meetings\n";
                    std::cout << "2. Logout\n";
                    std::cout << "3. Exit\n";
                    std::cout << "4. View meeting details associating a student\n";

                    int option;
                    std::cout << "Enter your choice: ";
                    std::cin >> option;
                    std::cin.ignore(); // Để bỏ qua newline sau khi nhập option

                    if (option == 1)
                    {
                        int meetingId;
                        std::string content, name, password, isMaleStr, isTeacherStr;
                        std::cout << "Enter content\nMeeting id: ";
                        std::cin >> meetingId;
                        std::cin.ignore(); // Để bỏ qua newline sau khi nhập option
                        command = "CHECK_MEETING_WITH_TEACHER/" + email + "/" + std::to_string(meetingId);
                        response = client.sendRequest(command);
                        if (response == "200;Teacher and meeting match")
                        {
                            std::cout << "Permission Accept\nEnter content (enter 'END' to finish):\n";

                            std::string content;
                            std::string line;

                            // Lặp lại để người dùng có thể nhập nhiều dòng
                            while (true)
                            {
                                std::getline(std::cin, line); // Đọc từng dòng
                                if (line == "END")            // Kết thúc khi người dùng nhập 'END'
                                    break;
                                content += line + "\n"; // Thêm dòng vào nội dung, giữ xuống dòng
                            }
                            command = "CREATE_CONTENT/" + std::to_string(meetingId) + "/" + content;
                            response = client.sendRequest(command);
                            // std::cout << response << std::endl;
                        }
                        else
                        {
                            std::cout << "You don't have permission for this meeting" << std::endl;
                        }
                    }
                    else if (option == 2)
                    {
                        // Đăng xuất
                        token = "";
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
                    else if (option == 4)
                    {
                        std::string studentName;
                        std::cout << "View your meeting details associating a student\n";
                        std::cout << "Enter student name: ";
                        std::getline(std::cin, studentName);
                        // std::cin.ignore(); // Để bỏ qua newline sau khi nhập option
                        command = "VIEW_MEETING_DETAILS_ASSOCIATING_STUDENT/" + email + "/" + studentName;
                        response = client.sendRequest(command);
                        // if (response == "200;Meeting details found")
                        // {
                        //     std::cout << "Meeting details found\n";
                        //     // Xử lý phản hồi và hiển thị thông tin
                        // }
                        // else
                        // {
                        //     std::cout << "You have no meeting assigned by this student\n";
                        // }
                    }
                    else
                    {
                        std::cout << "Invalid option. Try again.\n";
                    }
                }
            }
            else if (res[0] == "200;Login successful by student")
            {
                token = res[1];
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