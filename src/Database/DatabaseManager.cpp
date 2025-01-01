#include "DatabaseManager.h"
#include <pqxx/pqxx>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <vector>
// Có thể cần thêm thư viện cho mã hóa mật khẩu như OpenSSL

DatabaseManager::DatabaseManager(const std::string &connectionStr)
    : connectionString(connectionStr) {}

std::string DatabaseManager::hashPassword(const std::string &password)
{
    // Sử dụng phương thức mã hóa mật khẩu thực sự nếu cần
    return password; // Chỉ là placeholder, cần cải tiến
}

bool DatabaseManager::registerUser(const std::string &email, const std::string &name, bool isMale, const std::string &password, bool isTeacher)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        std::string hashedPassword = hashPassword(password);
        txn.exec_params(
            "INSERT INTO Users (email, name, is_male, password, is_teacher) VALUES ($1, $2, $3, $4, $5)",
            email, name, isMale, hashedPassword, isTeacher);

        txn.commit();
        std::cout << "User registered: " << email << std::endl;
        return true;
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

int DatabaseManager::loginUser(const std::string &email, const std::string &password)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        std::string hashedPassword = hashPassword(password);
        pqxx::result r = txn.exec_params(
            "SELECT is_teacher FROM Users WHERE email = $1 AND password = $2",
            email, hashedPassword);

        txn.commit();

        if (!r.empty())
        {
            bool isTeacher = r[0][0].as<bool>(); // Lấy giá trị is_teacher
            std::cout << "Login successful for user: " << email << std::endl;
            if (isTeacher)
            {
                return 2;
            }
            else
            {
                return 1;
            }
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
bool DatabaseManager::checkMeetingWithTeacher(const std::string &email, int meetingId)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM meetings WHERE meeting_id = $1 AND teacher_email = $2",
            meetingId, email);

        txn.commit();

        if (!r.empty())
        {
            int count = r[0][0].as<int>(); // Lấy giá trị COUNT(*)
            if (count == 1)
            {
                std::cout << "Meeting with teacher exists for user: " << email << std::endl;
                return true;
            }
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return false;
}
bool DatabaseManager::cancelMeeting(const std::string& meetingID) {
    try {
 // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        txn.exec_params(
            "DELETE FROM Meetings WHERE meeting_id = $1",
            meetingID
        );

        txn.commit();
        std::cout << "Meeting cancelled: " << meetingID << std::endl;
        return true;
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return false;
}
bool DatabaseManager::scheduleIndividualMeeting(const std::string& timeslot_id, const std::string& student_id, const std::string& type) {
    try {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        txn.exec_params( "INSERT INTO meeting_participants (meeting_id, student_id, type) VALUES ($1, $2, $3)",
            timeslot_id, student_id, type
        );

        txn.commit();
        std::cout << "Meeting scheduled: " << timeslot_id << std::endl;
        return true;
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return false;
}


bool DatabaseManager::createContent(int meetingId, const std::string &content)
{
    try
    { // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        txn.exec_params(
"INSERT INTO notes (meeting_id, note_content) VALUES ($1, $2)",
            meetingId, content);

        txn.commit();
        std::cout << "Content created for meeting ID: " << meetingId << std::endl;
        return true;
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

//Fetch thời gian rảnh của thầy từ DB
std::vector<std::pair<std::string, std::string>> DatabaseManager::getTeacherTimeSlots(int teacherId)
{
    std::vector<std::pair<std::string, std::string>> timeSlots;
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result result = txn.exec_params(
            "SELECT start_time, end_time FROM TeacherTimeSlots WHERE teacher_id = $1",
            teacherId);

        for (auto row : result)
        {
            timeSlots.emplace_back(row["start_time"].c_str(), row["end_time"].c_str());
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    }
    return timeSlots;
}

bool DatabaseManager::createTimeSlot(int teacherId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting)
{
    try
    {        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        txn.exec_params(           "INSERT INTO TeacherTimeSlots (teacher_id, start_time, end_time, is_group_meeting) VALUES ($1, $2, $3, $4)",
            teacherId, startTime, endTime, isGroupMeeting);

        txn.commit();
        return true;
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return false;
    }
}

bool DatabaseManager::editTimeSlot(int slotId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting)
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        txn.exec_params(
            "UPDATE TeacherTimeSlots SET start_time = $1, end_time = $2, is_group_meeting = $3 WHERE id = $4",
            startTime, endTime, isGroupMeeting, slotId);

        txn.commit();
        return true;
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return false;
    }
}

// #Fetch meeting schdule by week or date
std::vector<std::tuple<int, int, int, std::string, std::string, bool>> DatabaseManager::getMeetingsByDate(const std::string &date)
{
    std::vector<std::tuple<int, int, int, std::string, std::string, bool>> meetings;
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result result = txn.exec_params(
            "SELECT id, teacher_id, student_id, start_time, end_time, is_group_meeting FROM Meetings WHERE DATE(start_time) = $1",
            date);

        for (auto row : result)
        {
            meetings.emplace_back(
                row["id"].as<int>(),
                row["teacher_id"].as<int>(),
                row["student_id"].as<int>(),
                row["start_time"].c_str(),
                row["end_time"].c_str(),
                row["is_group_meeting"].as<bool>());
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    }
    return meetings;
}

std::vector<std::tuple<int, int,int, std::string, std::string, bool>> DatabaseManager::getMeetingsByWeek(const std::string &startDate, const std::string &endDate)
{
    std::vector<std::tuple<int, int,int, std::string, std::string, bool>> meetings;
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result result = txn.exec_params(
            "SELECT id, teacher_id, student_id, start_time, end_time, is_group_meeting FROM Meetings WHERE start_time BETWEEN $1 AND $2",
            startDate, endDate);

        for (auto row : result)
        {
            meetings.emplace_back(
                row["id"].as<int>(),
                row["teacher_id"].as<int>(),
                row["student_id"].as<int>(),
                row["start_time"].c_str(),
                row["end_time"].c_str(),
                row["is_group_meeting"].as<bool>());
        }
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    }
    return meetings;
}
