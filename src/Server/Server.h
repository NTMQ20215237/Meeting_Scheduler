#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <vector>

class Server
{
public:
    Server(int port);
    ~Server();
    void start();
    void stop();

private:
    int serverSocket;
    bool running;
    std::vector<std::string> split(const std::string &str, char delimiter);
    void handleClient(int clientSocket);
    std::string processRequest(int clientSocket, const std::string &request);                         // Updated signature
    std::string handleLogin(int clientSocket, const std::string &email, const std::string &password); // Updated signature
    std::string handleRegister(const std::string &email, const std::string &name, const std::string &password, bool is_male, bool is_teacher);
    std::string checkMeetingWithTeacher(const std::string &email, int meetingId);
    std::string handleCreateContent(int meetingId, const std::string &content);
    std::string handleViewMeetingDetailsAssociatingStudent(const std::string &email, const std::string &studentName);
    std::string handleDeclareNewAvailableTimeSlot(const std::string &token, const std::string &date, const std::string &start_time, const std::string &end_time);
    std::string handleLogout(int clientSocket); // Updated signature    std::string getAllStudents();
    std::string getAllTeachers();

    std::string getAllStudents();
    std::string createMeeting(int clientSocket, const std::string &token, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId);
    std::string getAllMeetings(int clientSocket);
    std::string viewMeetingDetail(int clientSocket, const std::string &meetingId);
    std::string deleteMeeting(int clientSocket, const std::string &meetingId);
    std::string editMeeting(const std::string meetingId, const std::string &meetingTitle, const std::string &startAt, const std::string &endAt, const std::string &isGroup, const std::vector<std::string> &studentsId);
    std::string handleViewAllAvailableTimeSlot(const std::string &token);
    std::string handleRemoveAvailableTimeSlot(const std::string &token, int order);
    std::string handleUpdateAvailableTimeSlot(const std::string &token, int order, const std::string &date, const std::string &start_time, const std::string &end_time);
    std::string handleViewAvailableTimeSlotWithTimeRange(const std::string &token, const std::string &start_date, const std::string &end_date);
    std::string handleViewTeacherAvailableTimeSlots(const std::string &token, const std::string &teacherEmail);
    std::string handleViewTeacherAvailableTimeSlotsInDateRange(const std::string &token, const std::string &teacherEmail, const std::string &start_date, const std::string &end_date);
    std::string getAllStudentMeetings( int clientSocket, const std::string &email);
    std::string viewStudentMeetingDetails(const std::string &meetingId);
};

#endif