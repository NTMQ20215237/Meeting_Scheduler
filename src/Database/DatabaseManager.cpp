#include "DatabaseManager.h"
#include <pqxx/pqxx>
#include <iostream>
#include <sstream>
#include <functional>
#include <algorithm>
// Có thể cần thêm thư viện cho mã hóa mật khẩu như OpenSSL

DatabaseManager::DatabaseManager(const std::string& connectionStr) 
    : connectionString(connectionStr) {}

std::string DatabaseManager::hashPassword(const std::string& password) {
    // Sử dụng phương thức mã hóa mật khẩu thực sự nếu cần
    return password;  // Chỉ là placeholder, cần cải tiến
}

bool DatabaseManager::registerUser(const std::string& email, const std::string& name, bool isMale, const std::string& password, bool isTeacher) {
    try {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::work txn(conn);

        std::string hashedPassword = hashPassword(password);
        txn.exec_params(
            "INSERT INTO Users (email, name, is_male, password, is_teacher) VALUES ($1, $2, $3, $4, $5)",
            email, name, isMale, hashedPassword, isTeacher
        );

        txn.commit();
        std::cout << "User registered: " << email << std::endl;
        return true;
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
        return false;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool DatabaseManager::loginUser(const std::string& email, const std::string& password) {
    try {
        // Khởi tạo kết nối tới cơ sở dữ liệu
        pqxx::connection conn(connectionString);
        pqxx::nontransaction txn(conn);

        std::string hashedPassword = hashPassword(password);
        pqxx::result r = txn.exec_params(
            "SELECT 1 FROM Users WHERE email = $1 AND password = $2",
            email, hashedPassword
        );

        txn.commit();

        if (!r.empty()) {
            std::cout << "Login successful for user: " << email << std::endl;
            return true;
        } 
    } catch (const pqxx::sql_error &e) {
        std::cerr << "SQL error: " << e.what() << ", in query: " << e.query() << std::endl;
    } catch (const std::exception &e) {
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

        txn.exec_params(
            "INSERT INTO meeting_participants (meeting_id, student_id, type) VALUES ($1, $2, $3)",
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

