#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <pqxx/pqxx>

class DatabaseManager {
public:
    DatabaseManager(const std::string& connectionStr);
    bool registerUser(const std::string& email, const std::string& name, bool isMale, const std::string& password, bool isTeacher);
    bool loginUser(const std::string& email, const std::string& password);
    bool cancelMeeting(const std::string& meetingID);
    bool scheduleIndividualMeeting(const std::string& timeslot_id, const std::string& student_id, const std::string& type);
        
private:
    std::string connectionString;
    std::string hashPassword(const std::string& password);
};

#endif