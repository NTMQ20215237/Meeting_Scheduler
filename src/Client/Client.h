#ifndef CLIENT_H
#define CLIENT_H

#include <string>
using namespace std;

class Client
{
public:
    Client(const std::string &serverIP, int port);
    ~Client();
    std::string sendRequest(const std::string &requestType, const std::string &request, std::string &response);
    void registerUser(const std::string &email, const std::string &name, bool isMale, const std::string &password, bool isTeacher);
    void loginUser(const std::string &email, const std::string &password);

    void viewTeacherTimeSlots(int teacherId);
    void createTimeSlot(int teacherId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting);
    void editTimeSlot(int slotId, const std::string &startTime, const std::string &endTime, bool isGroupMeeting);
    void deleteTimeSlot(int slotId);


    void createMeeting(const std::string title, const std::string teacherEmail, const std::string &startTime, const std::string &endTime, bool isGroupMeeting,std::vector<std::string> studentIds);
    void viewSpecificMeeting(int meetingId);
    void viewAllMeeting();
    void viewMeetingsByDate(const std::string &date);
    void viewMeetingsByWeek(const std::string &startDate, const std::string &endDate);
    void EditSpecificMeeting(int meetingId, const std::string &title, const std::string &teacherEmail, const std::string &startTime, const std::string &endTime, bool isGroupMeeting, std::vector<std::string> studentIds);
    void deleteSpecificMeeting(int meetingId);

private:
    int clientSocket;
    void connectToServer(const std::string &serverIP, int port);
};

#endif
