#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <pqxx/pqxx>

class DatabaseManager
{
public:
    DatabaseManager(const std::string &connectionStr);
    bool registerUser(const std::string &email, const std::string &name, bool isMale, const std::string &password, bool isTeacher);
    int loginUser(const std::string &email, const std::string &password);

private:
    std::string connectionString;
    std::string hashPassword(const std::string &password);
};

#endif