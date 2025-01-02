#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <pqxx/pqxx>

class DatabaseManager
{
public:
    DatabaseManager(const std::string &connectionStr);
    bool registerUser(const std::string &email, const std::string &name, bool isMale, const std::string &password, bool isTeacher);
    std::string loginUser(const std::string &email, const std::string &password);
    bool checkMeetingWithTeacher(const std::string &email, int meetingId);
    bool createContent(int meetingId, const std::string &content);
    std::string viewMeetingDetailsAssociatingStudent(const std::string &email, const std::string &studentName);
    std::string getAllStudents();
    std::string getAllTeachers();
    bool createMeeting(const std::string &email, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId);
    std::string getAllMeetings(const std::string &email);
    std::string viewMeetingDetail(const std::string &email, const std::string &meetingId);
    bool deleteMeeting(const std::string &meetingId);
    bool editMeeting(const std::string &meetingId, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId);

private:
    std::string connectionString;
    std::string hashPassword(const std::string &password);
};

#endif