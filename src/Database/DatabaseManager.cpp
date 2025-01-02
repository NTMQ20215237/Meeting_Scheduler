#include "DatabaseManager.h"
#include <pqxx/pqxx>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
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

std::string DatabaseManager::loginUser(const std::string &email, const std::string &password)
{
    boost::uuids::random_generator generator;
    boost::uuids::uuid token = generator();
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
            std::string tokenStr = boost::uuids::to_string(token);
            pqxx::nontransaction txn1(conn);
            txn1.exec_params(
                "UPDATE Users SET token = $1 WHERE email = $2",
                tokenStr, email);
            txn1.commit();
            if (isTeacher)
            {
                return "2/" + tokenStr;
            }
            else
            {
                return "1/" + tokenStr;
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
    return "0/";
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

std::string DatabaseManager::getAllStudents()
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec("SELECT id,name,email FROM Users WHERE is_teacher = false");

        txn.commit();

        std::stringstream resultStream1;
        std::stringstream resultStream2;
        for (const auto &row : r)
        {
            std::string id = row[0].as<std::string>();
            std::string name = row[1].as<std::string>();
            std::string email = row[2].as<std::string>();

            resultStream1 << "ID: " << id << ", Name: " << name << ", Email: " << email << std::endl;
            resultStream2 << id << "/";
        }

        return resultStream1.str() + '|' + resultStream2.str();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

std::string DatabaseManager::getAllTeachers()
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec("SELECT id,name,email FROM Users WHERE is_teacher = true");

        txn.commit();

        std::stringstream resultStream1;
        std::stringstream resultStream2;
        for (const auto &row : r)
        {
            std::string id = row[0].as<std::string>();
            std::string name = row[1].as<std::string>();
            std::string email = row[2].as<std::string>();

            resultStream1 << "ID: " << id << ", Name: " << name << ", Email: " << email << std::endl;
            resultStream2 << id << "/";
        }

        return resultStream1.str() + '|' + resultStream2.str();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "";
    }
}

bool DatabaseManager::createMeeting(const std::string &email, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId)
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result r = txn.exec_params(
            "INSERT INTO Meetings (title, start_at, end_at, teacher_email, is_group) VALUES ($1, $2, $3, $4, $5) RETURNING meeting_id",
            meetingTitle, startAt, endAt, email, isGroup);

        int meetingId = r[0][0].as<int>();

        for (const std::string &studentId : studentsId)
        {
            txn.exec_params(
                "INSERT INTO meeting_participants (meeting_id, student_id) VALUES ($1, $2)",
                meetingId, studentId);
        }

        txn.commit();
        std::cout << "Meeting created with ID: " << meetingId << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

std::string DatabaseManager::getAllMeetings(const std::string &email)
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT meeting_id,title,start_at,end_at, is_group FROM Meetings WHERE teacher_email = $1", email);

        txn.commit();

        std::stringstream resultStream1;
        std::stringstream resultStream2;
        for (const auto &row : r)
        {
            int meeting_id = row[0].as<int>();
            std::string title = row[1].as<std::string>();
            std::string start_at = row[2].as<std::string>();
            std::string end_at = row[3].as<std::string>();
            bool is_group = row[4].as<bool>();

            resultStream1 << "Meeting ID: " << meeting_id
                          << ", Title: " << title << std::endl
                          << ", Start at: " << start_at << std::endl
                          << ", End at: " << end_at << std::endl
                          << ", Is group: " << (is_group ? "true" : "false") << std::endl;
            resultStream2 << meeting_id << "/";
        }

        return resultStream1.str() + "|" + resultStream2.str();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "|";
    }
}

std::string DatabaseManager::viewMeetingDetail(const std::string &email, const std::string &meetingId)
{
    std::stringstream resultStream1;
    std::stringstream resultStream2;
    std::stringstream resultStream3;
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT title,start_at,end_at,is_group FROM Meetings WHERE teacher_email = $1 AND meeting_id = $2",
            email, meetingId);
        pqxx::result r1 = txn.exec_params(
            "SELECT u.name,u.email FROM users u JOIN meeting_participants mp ON u.id = mp.student_id WHERE mp.meeting_id = $1",
            meetingId);
        pqxx::result r2 = txn.exec_params(
            "SELECT u.name,u.email FROM users u WHERE u.email = $1", email);

        txn.commit();

        if (!r.empty())
        {
            std::string title = r[0][0].as<std::string>();
            std::string start_at = r[0][1].as<std::string>();
            std::string end_at = r[0][2].as<std::string>();
            bool is_group = r[0][3].as<bool>();

            resultStream1 << "Title: " << title
                          << ", Start at: " << start_at
                          << ", End at: " << end_at
                          << ", Is group: " << (is_group ? "true" : "false") << std::endl;

            resultStream2 << "Teacher: " << r2[0][0].as<std::string>() << ", Email: " << r2[0][1].as<std::string>() << std::endl;
            resultStream3 << "Participants: \n";
            for (const auto &row : r1)
            {
                resultStream3 << "Name: " << row[0].as<std::string>() << ", Email: " << row[1].as<std::string>() << std::endl;
            }

            resultStream1 << resultStream2.str() << resultStream3.str();
        }
        else
        {
            resultStream1 << "No meeting found for the specified teacher and meeting ID." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        resultStream1 << "No meeting found for the specified teacher and meeting ID." << std::endl;
    }
    return resultStream1.str(); // Trả về kết quả dưới dạng chuỗi
}

bool DatabaseManager::deleteMeeting(const std::string &meetingId)
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result r = txn.exec_params(
            "DELETE FROM Meetings WHERE meeting_id = $1",
            meetingId);

        txn.commit();
        std::cout << "Meeting deleted with ID: " << meetingId << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::editMeeting(const std::string &meetingId, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId)
{
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        pqxx::result r = txn.exec_params(
            "UPDATE Meetings SET title = $1, start_at = $2, end_at = $3, is_group = $4 WHERE meeting_id = $5",
            meetingTitle, startAt, endAt, isGroup, meetingId);

        txn.exec_params(
            "DELETE FROM meeting_participants WHERE meeting_id = $1",
            meetingId);

        for (const std::string &studentId : studentsId)
        {
            txn.exec_params(
                "INSERT INTO meeting_participants (meeting_id, student_id) VALUES ($1, $2)",
                meetingId, studentId);
        }

        txn.commit();
        std::cout << "Meeting edited with ID: " << meetingId << std::endl;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

std::string DatabaseManager::declareNewAvailableTimeSlot(const std::string &token, const std::string &date, const std::string &start_time, const std::string &end_time)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy email của giáo viên từ token
        pqxx::result teacherResult = txn.exec_params(
            "SELECT email FROM users WHERE token = $1",
            token);

        if (teacherResult.empty())
        {
            return "401: Unauthorized";
        }

        std::string teacherEmail = teacherResult[0]["email"].as<std::string>();

        // Kiểm tra xem có thời gian chồng chéo hay không (chuyển đổi chuỗi sang timestamp để so sánh)
        pqxx::result existingSlots = txn.exec_params(
            "SELECT start_time, finish_time FROM slots WHERE teacher_email = $1",
            teacherEmail);

        for (const auto &row : existingSlots)
        {
            std::string existingStartTime = row["start_time"].as<std::string>();
            std::string existingEndTime = row["finish_time"].as<std::string>();

            // Chuyển đổi start_time và finish_time từ string sang timestamp trong SQL
            pqxx::result overlapCheck = txn.exec_params(
                "SELECT 1 FROM slots "
                "WHERE teacher_email = $1 "
                "AND (start_time < $2::timestamp AND finish_time > $3::timestamp)",
                teacherEmail, date + " " + end_time, date + " " + start_time);

            if (!overlapCheck.empty())
            {
                return "409: Time slot conflict with existing slot";
            }
        }

        // Nếu không có xung đột, chèn dữ liệu mới vào bảng slots
        txn.exec_params(
            "INSERT INTO slots (teacher_email, start_time, finish_time) "
            "VALUES ($1, $2, $3)",
            teacherEmail,
            date + " " + start_time,
            date + " " + end_time);

        txn.commit(); // Xác nhận giao dịch

        return "200: Slot declared successfully";
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return "500: Internal Server Error";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "500: Internal Server Error";
    }
}

std::string DatabaseManager::viewAllAvailableTimeSlots(const std::string &token)
{
    std::stringstream resultStream; // Để lưu trữ kết quả dưới dạng chuỗi

    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy email của giáo viên từ token
        pqxx::result teacherResult = txn.exec_params(
            "SELECT email FROM users WHERE token = $1",
            token);

        if (teacherResult.empty())
        {
            return "401: Unauthorized";
        }

        std::string teacherEmail = teacherResult[0]["email"].as<std::string>();

        // Lấy tất cả các slot của giáo viên
        pqxx::result slotsResult = txn.exec_params(
            "SELECT start_time, finish_time FROM slots WHERE teacher_email = $1 ORDER BY start_time",
            teacherEmail);

        if (!slotsResult.empty())
        {
            int i = 1;
            resultStream << "200: Available time slots:" << std::endl;
            for (const auto &row : slotsResult)
            {
                std::string start_time = row["start_time"].as<std::string>();
                std::string finish_time = row["finish_time"].as<std::string>();

                resultStream << i << ".Start time: " << start_time
                             << ", Finish time: " << finish_time << std::endl;
                i++;
            }
        }
        else
        {
            resultStream << "404: No slots found for the specified teacher." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return resultStream.str(); // Trả về kết quả dưới dạng chuỗi
}
std::string DatabaseManager::removeAvailableTimeSlot(const std::string &token, int order)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy email của giáo viên từ token
        pqxx::result teacherResult = txn.exec_params(
            "SELECT email FROM users WHERE token = $1",
            token);

        if (teacherResult.empty())
        {
            return "401: Unauthorized";
        }

        std::string teacherEmail = teacherResult[0]["email"].as<std::string>();

        // Lấy danh sách các slot và kiểm tra thứ tự hợp lệ
        pqxx::result slotsResult = txn.exec_params(
            "SELECT id FROM slots WHERE teacher_email = $1 ORDER BY start_time",
            teacherEmail);

        if (slotsResult.empty() || order > static_cast<int>(slotsResult.size()))
        {
            return "400: Invalid slot order";
        }

        // Xóa slot theo thứ tự
        int slotId = slotsResult[order - 1]["id"].as<int>();
        txn.exec_params("DELETE FROM slots WHERE id = $1", slotId);

        txn.commit(); // Commit transaction

        return "200: Time slot removed successfully";
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return "500: Internal Server Error";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "500: Internal Server Error";
    }
}
std::string DatabaseManager::updateAvailableTimeSlot(const std::string &token, int order, const std::string &date, const std::string &start_time, const std::string &end_time)
{
    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy email của giáo viên từ token
        pqxx::result teacherResult = txn.exec_params(
            "SELECT email FROM users WHERE token = $1",
            token);

        if (teacherResult.empty())
        {
            return "401: Unauthorized";
        }

        std::string teacherEmail = teacherResult[0]["email"].as<std::string>();

        // Lấy danh sách các slot và kiểm tra thứ tự hợp lệ
        pqxx::result slotsResult = txn.exec_params(
            "SELECT id FROM slots WHERE teacher_email = $1 ORDER BY start_time",
            teacherEmail);

        if (slotsResult.empty() || order > static_cast<int>(slotsResult.size()))
        {
            return "400: Invalid slot order";
        }

        // Cập nhật slot theo thứ tự
        int slotId = slotsResult[order - 1]["id"].as<int>();
        txn.exec_params("UPDATE slots SET start_time = $1, finish_time = $2 WHERE id = $3", date + " " + start_time, date + " " + end_time, slotId);

        txn.commit(); // Commit transaction

        return "200: Time slot updated successfully";
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return "500: Internal Server Error";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "500: Internal Server Error";
    }
}

std::string DatabaseManager::viewAvailableTimeSlotWithTimeRange(const std::string &token, const std::string &start_date, const std::string &end_date)
{
    std::stringstream resultStream; // Để lưu trữ kết quả dưới dạng chuỗi

    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy email của giáo viên từ token
        pqxx::result teacherResult = txn.exec_params(
            "SELECT email FROM users WHERE token = $1",
            token);

        if (teacherResult.empty())
        {
            return "401: Unauthorized";
        }

        std::string teacherEmail = teacherResult[0]["email"].as<std::string>();

        // Lấy tất cả các slot của giáo viên trong khoảng thời gian
        pqxx::result slotsResult = txn.exec_params(
            "SELECT start_time, finish_time FROM slots WHERE teacher_email = $1 AND start_time >= $2 AND finish_time <= $3 ORDER BY start_time",
            teacherEmail, start_date, end_date + " 23:59:59");

        if (!slotsResult.empty())
        {
            int i = 1;
            resultStream << "200: Available time slots in the specified range:" << std::endl;
            for (const auto &row : slotsResult)
            {
                std::string start_time = row["start_time"].as<std::string>();
                std::string finish_time = row["finish_time"].as<std::string>();

                resultStream << i << ".Start time: " << start_time
                             << ", Finish time: " << finish_time << std::endl;
                i++;
            }
        }
        else
        {
            resultStream << "404: No slots found for the specified teacher in the specified range." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const pqxx::sql_error &e)
    {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return "500: Internal Server Error";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "500: Internal Server Error";
    }
    return resultStream.str(); // Trả về kết quả dưới dạng chuỗi
}
std::string DatabaseManager::viewTeacherAvailableTimeSlots(const std::string &token, const std::string &teacherEmail)
{
    std::stringstream resultStream; // Để lưu trữ kết quả dưới dạng chuỗi

    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy tất cả các slot của giáo viên
        pqxx::result slotsResult = txn.exec_params(
            "SELECT start_time, finish_time FROM slots WHERE teacher_email = $1 ORDER BY start_time",
            teacherEmail);

        if (!slotsResult.empty())
        {
            int i = 1;
            resultStream << "200: Available time slots for the specified teacher:" << std::endl;
            for (const auto &row : slotsResult)
            {
                std::string start_time = row["start_time"].as<std::string>();
                std::string finish_time = row["finish_time"].as<std::string>();

                resultStream << i << ".Start time: " << start_time
                             << ", Finish time: " << finish_time << std::endl;
                i++;
            }
        }
        else
        {
            resultStream << "404: No slots found for the specified teacher." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return resultStream.str(); // Trả về kết quả dưới dạng chuỗi
}
std::string DatabaseManager::viewTeacherAvailableTimeSlotsInDateRange(const std::string &token, const std::string &teacherEmail, const std::string &start_date, const std::string &end_date)
{
    std::stringstream resultStream; // Để lưu trữ kết quả dưới dạng chuỗi

    try
    {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        // Kiểm tra tính hợp lệ của token
        pqxx::result r = txn.exec_params(
            "SELECT COUNT(*) FROM users WHERE token = $1",
            token);

        if (r.empty() || r[0][0].as<int>() != 1)
        {
            return "401: Unauthorized";
        }

        // Lấy tất cả các slot của giáo viên trong khoảng thời gian
        pqxx::result slotsResult = txn.exec_params(
            "SELECT start_time, finish_time FROM slots WHERE teacher_email = $1 AND start_time >= $2 AND finish_time <= $3 ORDER BY start_time",
            teacherEmail, start_date, end_date + " 23:59:59");

        if (!slotsResult.empty())
        {
            int i = 1;
            resultStream << "200: Available time slots for the specified teacher in the specified range:" << std::endl;
            for (const auto &row : slotsResult)
            {
                std::string start_time = row["start_time"].as<std::string>();
                std::string finish_time = row["finish_time"].as<std::string>();

                resultStream << i << ".Start time: " << start_time
                             << ", Finish time: " << finish_time << std::endl;
                i++;
            }
        }
        else
        {
            resultStream << "404: No slots found for the specified teacher in the specified range." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return resultStream.str(); // Trả về kết quả dưới dạng chuỗi
}

std::string DatabaseManager::getAllStudentMeetings(int clientSocket,const std::string &email){
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT id FROM Users WHERE email = $1", email);
        pqxx::result r1 = txn.exec_params(
            "SELECT meeting_id,title,teacher_email,start_at,end_at,is_group FROM Meetings WHERE meeting_id IN "
            "(SELECT meeting_id FROM meeting_participants WHERE student_id = $1)", r[0][0].as<int>());
        txn.commit();

        std::stringstream resultStream1;
        std::stringstream resultStream2;
        for (const auto &row : r1)
        {
            int meeting_id = row[0].as<int>();
            std::string title = row[1].as<std::string>();
            std::string teacher_email = row[2].as<std::string>();
            std::string start_at = row[3].as<std::string>();
            std::string end_at = row[4].as<std::string>();
            bool is_group = row[5].as<bool>();

            resultStream1 << "Meeting ID: " << meeting_id
                          << ", Title: " << title << std::endl
                            << ", Teacher Email: " << teacher_email << std::endl
                          << ", Start at: " << start_at << std::endl
                          << ", End at: " << end_at << std::endl
                          << ", Is group: " << (is_group ? "true" : "false") << std::endl;
            resultStream2 << meeting_id << "/";
        }

        return resultStream1.str() + "|" + resultStream2.str();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return "|";
    }
}

std::string DatabaseManager::viewStudentMeetingDetails(const std::string &meetingId)
{
    std::stringstream resultStream1;
    std::stringstream resultStream2;
    std::stringstream resultStream3;
    try
    {
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        pqxx::result r = txn.exec_params(
            "SELECT title,start_at,end_at,is_group FROM Meetings WHERE meeting_id = $1",
            meetingId);
        pqxx::result r1 = txn.exec_params(
            "SELECT u.name,u.email FROM users u JOIN meeting_participants mp ON u.id = mp.student_id WHERE mp.meeting_id = $1",
            meetingId);
        pqxx::result r2 = txn.exec_params(
            "SELECT u.name,u.email FROM users u JOIN Meetings m ON u.email = m.teacher_email WHERE m.meeting_id = $1", meetingId);

        txn.commit();

        if (!r.empty())
        {
            std::string title = r[0][0].as<std::string>();
            std::string start_at = r[0][1].as<std::string>();
            std::string end_at = r[0][2].as<std::string>();
            bool is_group = r[0][3].as<bool>();

            resultStream1 << "Title: " << title
                          << ", Start at: " << start_at
                          << ", End at: " << end_at
                          << ", Is group: " << (is_group ? "true" : "false") << std::endl;

            resultStream2 << "Teacher: " << r2[0][0].as<std::string>() << ", Email: " << r2[0][1].as<std::string>() << std::endl;
            resultStream3 << "Participants: \n";
            for (const auto &row : r1)
            {
                resultStream3 << "Name: " << row[0].as<std::string>() << ", Email: " << row[1].as<std::string>() << std::endl;
            }

            resultStream1 << resultStream2.str() << resultStream3.str();
        }
        else
        {
            resultStream1 << "No meeting found for the specified meeting ID." << std::endl;
        }

        txn.commit(); // Commit the transaction
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what()

        << std::endl;
        resultStream1 << "No meeting found for the specified meeting ID." << std::endl;
    }
    return resultStream1.str(); // Trả về kết quả dưới dạng chuỗi
}
