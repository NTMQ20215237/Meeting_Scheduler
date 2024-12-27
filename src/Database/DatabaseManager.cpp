#include "DatabaseManager.h"
#include <pqxx/pqxx>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
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
bool DatabaseManager::createContent(int meetingId, const std::string &content)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
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
std::string DatabaseManager::viewMeetingDetailsAssociatingStudent(const std::string &email, const std::string &studentName)
{
    std::stringstream resultStream; // Để lưu trữ kết quả dưới dạng chuỗi

    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT m.meeting_id,n.note_content,m.teacher_email,s.email AS student_email FROM Meetings m JOIN Users u ON m.teacher_email = u.email JOIN meeting_participants mp ON m.meeting_id = mp.meeting_id JOIN Users s ON mp.student_id = s.id LEFT JOIN Notes n ON m.meeting_id = n.meeting_id WHERE u.email = $1 AND s.name = $2 ;",
            email, studentName);

        txn.commit();

        if (!r.empty())
        {
            for (const auto &row : r)
            {
                int meeting_id = row[0].as<int>();                                                   // meeting_id
                std::string note_content = row[1].is_null() ? "No notes" : row[1].as<std::string>(); // Note, or "No notes"
                std::string teacher_email = row[2].as<std::string>();                                // teacher_email
                std::string student_email = row[3].as<std::string>();                                // student_email

                resultStream << "Meeting ID: " << meeting_id
                             << ", Notes: " << note_content
                             << ", Teacher Email: " << teacher_email
                             << ", Student Email: " << student_email << std::endl;
            }
        }
        else
        {
            resultStream << "No meetings found for the specified teacher and student." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return resultStream.str(); // Trả về kết quả dưới dạng chuỗi
}