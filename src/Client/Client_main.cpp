#include "Client.h"
#include <iostream>
#include <iomanip>
#include <regex>
#include <string>
#include <vector>
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
                    std::cout << "5. Declare new available time slot\n";
                    std::cout << "6. Remove available time slot\n";
                    std::cout << "7. Edit available time slot\n";
                    std::cout << "8. View all available time slot\n";
                    std::cout << "9. View available time slot in a specific time range\n";

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
                        std::cout << response << std::endl;
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
                    else if (option == 5)
                    {
                        // Declare new available time slot
                        std::string date, start_time, end_time;
                        std::cout << "Declare new available time slot\n";

                        // Validate date input
                        while (true)
                        {
                            std::cout << "Enter date (YYYY-MM-DD): ";
                            std::getline(std::cin, date);
                            std::regex date_pattern(R"(\d{4}-\d{2}-\d{2})");
                            if (std::regex_match(date, date_pattern))
                            {
                                // Check if the date is valid
                                std::tm tm = {};
                                std::istringstream ss(date);
                                if (ss >> std::get_time(&tm, "%Y-%m-%d"))
                                    break;
                            }
                            std::cout << "Invalid date format! Please try again.\n";
                        }

                        // Validate start time input
                        while (true)
                        {
                            std::cout << "Enter start time (HH:MM): ";
                            std::getline(std::cin, start_time);
                            std::regex time_pattern(R"(\d{2}:\d{2})");
                            if (std::regex_match(start_time, time_pattern))
                            {
                                // Check if the time is valid
                                int hour, minute;
                                char sep;
                                std::istringstream ss(start_time);
                                if (ss >> hour >> sep >> minute && sep == ':' && hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
                                    break;
                            }
                            std::cout << "Invalid start time format! Please try again.\n";
                        }

                        // Validate end time input
                        while (true)
                        {
                            std::cout << "Enter end time (HH:MM): ";
                            std::getline(std::cin, end_time);
                            std::regex time_pattern(R"(\d{2}:\d{2})");
                            if (std::regex_match(end_time, time_pattern))
                            {
                                // Check if the time is valid
                                int hour, minute;
                                char sep;
                                std::istringstream ss(end_time);
                                if (ss >> hour >> sep >> minute && sep == ':' && hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
                                    break;
                            }
                            std::cout << "Invalid end time format! Please try again.\n";
                        }

                        command = "DECLARE_NEW_AVAILABLE_TIME_SLOT/" + token + "/" + date + "/" + start_time + "/" + end_time;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;
                    }
                    else if (option == 6)
                    {
                        int order;
                        std::cout << "\nChoose the order of the time slot to remove:\n";

                        // Yêu cầu xem tất cả các time slot hiện tại
                        command = "VIEW_ALL_AVAILABLE_TIME_SLOT/" + token;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;

                        // Nhập thứ tự slot để xóa
                        std::cout << "Enter the slot order to remove: ";
                        while (true)
                        {
                            std::cin >> order;
                            if (order <= 0)
                            {
                                std::cout << "Invalid order number. Please try again." << std::endl;
                            }
                            else
                            {
                                break;
                            }
                        }

                        // Gửi yêu cầu xóa
                        command = "REMOVE_AVAILABLE_TIME_SLOT/" + token + "/" + std::to_string(order);
                        response = client.sendRequest(command);

                        // Hiển thị phản hồi
                        std::cout << response << std::endl;
                    }
                    else if (option == 7)
                    {
                        int order;
                        std::cout << "\nChoose the order of the time slot to UPDATE:\n";

                        // Yêu cầu xem tất cả các time slot hiện tại
                        command = "VIEW_ALL_AVAILABLE_TIME_SLOT/" + token;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;

                        // Nhập thứ tự slot để xóa
                        std::cout << "Enter the slot order to UPDATE: ";
                        while (true)
                        {
                            std::cin >> order;
                            if (order <= 0)
                            {
                                std::cout << "Invalid order number. Please try again." << std::endl;
                            }
                            else
                            {
                                break;
                            }
                        }

                        std::string date, start_time, end_time;
                        std::cout << "Update time slot:\n";
                        // Validate date input
                        while (true)
                        {
                            std::cin.ignore();
                            std::cout << "Enter date (YYYY-MM-DD): ";
                            std::getline(std::cin, date);
                            std::regex date_pattern(R"(\d{4}-\d{2}-\d{2})");
                            if (std::regex_match(date, date_pattern))
                            {
                                // Check if the date is valid
                                std::tm tm = {};
                                std::istringstream ss(date);
                                if (ss >> std::get_time(&tm, "%Y-%m-%d"))
                                    break;
                            }
                            std::cout << "Invalid date format! Please try again.\n";
                        }
                        // Validate start time input
                        while (true)
                        {
                            std::cout << "Enter start time (HH:MM): ";
                            std::getline(std::cin, start_time);
                            std::regex time_pattern(R"(\d{2}:\d{2})");
                            if (std::regex_match(start_time, time_pattern))
                            {
                                // Check if the time is valid
                                int hour, minute;
                                char sep;
                                std::istringstream ss(start_time);
                                if (ss >> hour >> sep >> minute && sep == ':' && hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
                                    break;
                            }
                            std::cout << "Invalid start time format! Please try again.\n";
                        }

                        // Validate end time input
                        while (true)
                        {
                            std::cout << "Enter end time (HH:MM): ";
                            std::getline(std::cin, end_time);
                            std::regex time_pattern(R"(\d{2}:\d{2})");
                            if (std::regex_match(end_time, time_pattern))
                            {
                                // Check if the time is valid
                                int hour, minute;
                                char sep;
                                std::istringstream ss(end_time);
                                if (ss >> hour >> sep >> minute && sep == ':' && hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
                                    break;
                            }
                            std::cout << "Invalid end time format! Please try again.\n";
                        }

                        command = "UPDATE_TIME_SLOT/" + token + "/" + std::to_string(order) + "/" + date + "/" + start_time + "/" + end_time;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;
                    }
                    else if (option == 8)
                    {
                        // View all available time slot
                        std::cout << "View all available time slot:\n";
                        command = "VIEW_ALL_AVAILABLE_TIME_SLOT/" + token;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;
                    }
                    else if (option == 9)
                    {
                        // View available time slot in a specific time range
                        std::string start_date, end_date;
                        std::cout << "View available time slot in a specific time range\n";
                        while (true)
                        {
                            std::cout << "Enter start date (YYYY-MM-DD): ";
                            std::getline(std::cin, start_date);
                            std::regex date_pattern(R"(\d{4}-\d{2}-\d{2})");
                            if (std::regex_match(start_date, date_pattern))
                            {
                                // Check if the date is valid
                                std::tm tm = {};
                                std::istringstream ss(start_date);
                                if (ss >> std::get_time(&tm, "%Y-%m-%d"))
                                    break;
                            }
                            std::cout << "Invalid date format! Please try again.\n";
                        }
                        // Validate end date input
                        while (true)
                        {
                            std::cout << "Enter end date (YYYY-MM-DD): ";
                            std::getline(std::cin, end_date);
                            std::regex date_pattern(R"(\d{4}-\d{2}-\d{2})");
                            if (std::regex_match(end_date, date_pattern))
                            {
                                // Check if the date is valid
                                std::tm tm = {};
                                std::istringstream ss(end_date);
                                if (ss >> std::get_time(&tm, "%Y-%m-%d"))
                                    break;
                            }
                            std::cout << "Invalid date format! Please try again.\n";
                        }
                        command = "VIEW_AVAILABLE_TIME_SLOT_WITH_TIME_RANGE/" + token + "/" + start_date + "/" + end_date;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;
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
                std::cout << response << std::endl;
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