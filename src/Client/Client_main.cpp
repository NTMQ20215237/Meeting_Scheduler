#include "Client.h"
#include <iostream>
#include <string>
#include<vector>
#include <set>
#include <algorithm>
bool parseBoolean(const std::string &str)
{
    return str == "true" || str == "1" || str == "yes";
}
bool isValidID(const std::string& id, const std::vector<std::string>& studentsID) {
    return std::find(studentsID.begin(), studentsID.end(), id) != studentsID.end();
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
                    std::cout << "5. Create Meeting\n";
                    std::cout << "6. View Meeting\n";
                    std::cout << "7. Edit Meeting\n";
                    std::cout << "8. Delete Meeting\n";
                    std::cout << "9. View All Students\n";
                    std::cout << "10. View All Teachers\n";


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
                    else if (option == 5)
                    {
                        std::string meetingTitle, startAt, endAt, isGroup;
                        std::cout << "Enter title of the meeting: ";
                        std::getline(std::cin, meetingTitle);
                        std::cout << "Enter start time (YYYY-MM-DD HH:MM): ";
                        std::getline(std::cin, startAt);
                        startAt += ":00";
                        std::cout << "Enter end time (YYYY-MM-DD HH:MM): ";
                        std::getline(std::cin, endAt);
                        endAt += ":00";
                        std::cout << "Is this a group meeting? (true/false): ";
                        std::getline(std::cin, isGroup);
                        
                        std::string res = client.sendRequest("GET_ALL_STUDENTS/");
                        std::vector<std::string> students = client.split(res, '|');
                        std::vector<std::string> studentsID = client.split(students[1], '/');

                        std::cout << "List of students: \n";
                        std::cout << students[0] << std::endl;

                        std::cout << "Enter student IDs: ";
                        std::set<std::string> enteredIDs; 
                        std::string id;
                        while (std::cin >> id) {
                            if (isValidID(id, studentsID)) {
                                if (enteredIDs.find(id) == enteredIDs.end()) {
                                    enteredIDs.insert(id);
                                    if (isGroup == "false"){
                                        break;
                                    }
                                } else {
                                    std::cout << "ID " << id << " is already entered. Try another one: ";
                                }
                            } else {
                                std::cout << "ID " << id << " is not valid. Try again: ";
                            }

                            if (std::cin.peek() == '\n') {
                                break;
                            }
                        }
                        command = "CREATE_MEETING/" + token + "/" + meetingTitle + "/" + startAt + "/" + endAt + "/" + isGroup + "/";
                        command += std::to_string(enteredIDs.size()) + "/";
                        for (const std::string& id : enteredIDs) {
                            command += id + "/";
                        }
                        // std::cout<<command<<std::endl;

                        response = client.sendRequest(command);
                        std::cout << response << std::endl;
                        
                    }
                    else if (option == 6)
                    {
                        std::string res = client.sendRequest("GET_ALL_MEETINGS/");
                        if (res=="|"){
                            std::cout << "No meeting found\n";
                            continue;
                        }
                        std::string meetID;
                        std::vector<std::string> meetings = client.split(res, '|');
                        if (meetings.size() < 2) {
                            std::cout << "Error: Unexpected response format." << std::endl;
                            continue;
                        }
                        std::cout<< "List of meetings: \n";
                        std::cout << meetings[0] << std::endl;
                        std::vector<std::string> meetingsID = client.split(meetings[1], '/');
                        while (true){
                            std::cout << "Enter meeting ID to view details: ";
                            std::getline(std::cin, meetID);
                            if (std::find(meetingsID.begin(), meetingsID.end(), meetID) != meetingsID.end()){
                                break;
                            }
                            else{
                                std::cout << "Invalid meeting ID. Try again.\n";
                            }
                        }
                        command = "VIEW_MEETING_DETAILS/" + meetID;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;


                    }
                    else if (option == 7)
                    {
                        std::string res = client.sendRequest("GET_ALL_MEETINGS/");
                        std::string meetID;
                        std::vector<std::string> meetings = client.split(res, '|');
                        if (meetings.size() < 2) {
                            std::cerr << "Error: Unexpected response format." << std::endl;
                        }
                        std::cout<< "List of meetings: \n";
                        std::cout << meetings[0] << std::endl;
                        std::vector<std::string> meetingsID = client.split(meetings[1], '/');
                        while (true){
                            std::cout << "Enter meeting ID to edit: ";
                            std::getline(std::cin, meetID);
                            if (std::find(meetingsID.begin(), meetingsID.end(), meetID) != meetingsID.end()){
                                break;
                            }
                            else{
                                std::cout << "Invalid meeting ID. Try again.\n";
                            }
                        }
                        std::string meetingTitle, startAt, endAt, isGroup;
                        std::cout << "Enter title of the meeting: ";
                        std::getline(std::cin, meetingTitle);
                        std::cout << "Enter start time (YYYY-MM-DD HH:MM): ";
                        std::getline(std::cin, startAt);
                        startAt += ":00";
                        std::cout << "Enter end time (YYYY-MM-DD HH:MM): ";
                        std::getline(std::cin, endAt);
                        endAt += ":00";
                        std::cout << "Is this a group meeting? (true/false): ";
                        std::getline(std::cin, isGroup);
                        
                        std::string res3 = client.sendRequest("GET_ALL_STUDENTS/");
                        std::vector<std::string> students = client.split(res3, '|');
                        std::vector<std::string> studentsID = client.split(students[1], '/');

                        std::cout << "List of students: \n";
                        std::cout << students[0] << std::endl;

                        std::cout << "Enter student IDs: ";
                        std::set<std::string> enteredIDs; 
                        std::string id;
                        while (std::cin >> id) {
                            if (isValidID(id, studentsID)) {
                                if (enteredIDs.find(id) == enteredIDs.end()) {
                                    enteredIDs.insert(id);
                                    if (isGroup == "false"){
                                        break;
                                    }
                                } else {
                                    std::cout << "ID " << id << " is already entered. Try another one: ";
                                }
                            } else {
                                std::cout << "ID " << id << " is not valid. Try again: ";
                            }

                            if (std::cin.peek() == '\n') {
                                break;
                            }
                        }

                        command = "EDIT_MEETING/" + meetID + "/" + meetingTitle + "/" + startAt + "/" + endAt + "/" + isGroup + "/";
                        command += std::to_string(enteredIDs.size()) + "/";
                        for (const std::string& id : enteredIDs) {
                            command += id + "/";
                        }
                        response = client.sendRequest(command);

                    }
                    else if (option == 8)
                    {
                        std::string res = client.sendRequest("GET_ALL_MEETINGS/");
                        std::string meetID;
                        std::vector<std::string> meetings = client.split(res, '|');
                        if (meetings.size() < 2) {
                            std::cerr << "Error: Unexpected response format." << std::endl;
                        }
                        std::cout<< "List of meetings: \n";
                        std::cout << meetings[0] << std::endl;
                        std::vector<std::string> meetingsID = client.split(meetings[1], '/');
                        while (true){
                            std::cout << "Enter meeting ID to delete: ";
                            std::getline(std::cin, meetID);
                            if (std::find(meetingsID.begin(), meetingsID.end(), meetID) != meetingsID.end()){
                                break;
                            }
                            else{
                                std::cout << "Invalid meeting ID. Try again.\n";
                            }
                        }
                        command = "DELETE_MEETING/" + meetID;
                        response = client.sendRequest(command);
                        std::cout << response << std::endl;

                    }
                    else if (option == 9)
                    {
                        std::string res = client.sendRequest("GET_ALL_STUDENTS/");
                        std::vector<std::string> students = client.split(res, '|');
                        std::cout<< "List of students: \n";
                        std::cout << students[0] << std::endl;
                    }
                    else if (option == 10)
                    {
                        std::string res = client.sendRequest("GET_ALL_TEACHERS/");
                        std::vector<std::string> teachers = client.split(res, '|');
                        std::cout<< "List of teachers: \n";
                        std::cout << teachers[0] << std::endl;
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