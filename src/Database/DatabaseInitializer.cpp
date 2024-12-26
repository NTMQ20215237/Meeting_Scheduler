#include <pqxx/pqxx>
#include <iostream>
#include <string>

class DatabaseInitializer
{
private:
    std::string dbName = "meeting_scheduler";                                                             // Tên database
    std::string adminConnectionStr = "dbname=postgres user=admin password=secret host=localhost";         // Kết nối admin
    std::string userConnectionStr = "dbname=meeting_scheduler user=admin password=secret host=localhost"; // Kết nối database

    void createDatabaseIfNotExists()
    {
        try
        {
            pqxx::connection adminConn(adminConnectionStr);

            pqxx::work txn(adminConn);
            std::string query = "SELECT 1 FROM pg_database WHERE datname = '" + dbName + "';";
            pqxx::result result = txn.exec(query);

            if (result.empty())
            {
                std::cout << "Database not found. Creating database: " << dbName << std::endl;

                // Đóng transaction và sử dụng kết nối để tạo database
                txn.commit();           // Kết thúc transaction
                adminConn.disconnect(); // Ngắt kết nối để thực hiện lệnh không thuộc transaction block

                // Tạo kết nối mới và thực hiện CREATE DATABASE
                pqxx::connection newConn(adminConnectionStr);
                pqxx::nontransaction createTxn(newConn);
                createTxn.exec("CREATE DATABASE " + dbName + ";");
                std::cout << "Database created successfully." << std::endl;
            }
            else
            {
                std::cout << "Database already exists: " << dbName << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error while creating database: " << e.what() << std::endl;
            throw;
        }
    }

    void createTablesIfNotExists()
    {
        try
        {
            pqxx::connection userConn(userConnectionStr);
            pqxx::work txn(userConn);

            // Bảng Users
            txn.exec(
                "CREATE TABLE IF NOT EXISTS Users ("
                "id SERIAL PRIMARY KEY,"
                "email VARCHAR(255) UNIQUE NOT NULL,"
                "name VARCHAR(255) NOT NULL,"
                "is_male BOOLEAN NOT NULL,"
                "password VARCHAR(255) NOT NULL,"
                "is_teacher BOOLEAN NOT NULL"
                ");");

            // Bảng Meetings
            txn.exec(
                "CREATE TABLE IF NOT EXISTS Meetings ("
                "meeting_id SERIAL PRIMARY KEY,"
                "title VARCHAR(255),"
                "teacher_email VARCHAR(255) REFERENCES Users(email),"
                "start_at TIMESTAMP,"
                "end_at TIMESTAMP,"
                "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                "updated_at TIMESTAMP,"
                "is_group BOOLEAN NOT NULL"
                ");");

            // Bảng Slots
            txn.exec(
                "CREATE TABLE IF NOT EXISTS Slots ("
                "id SERIAL PRIMARY KEY,"
                "teacher_email VARCHAR(255) REFERENCES Users(email),"
                "meeting INT REFERENCES Meetings(meeting_id),"
                "start_time TIMESTAMP NOT NULL,"
                "finish_time TIMESTAMP NOT NULL"
                ");");
            txn.exec(
                "CREATE TABLE IF NOT EXISTS Notes ("
                "id SERIAL Primary Key,"
                "meeting_id INT references Meetings(meeting_id) ON DELETE CASCADE,"
                "note_content TEXT NOT NULL,"
                "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                "update_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                ");");
            txn.exec(
                "CREATE TABLE meeting_participants ("
                "id SERIAL PRIMARY KEY,"
                "meeting_id INT REFERENCES Meetings(meeting_id) ON DELETE CASCADE,"
                "student_id INT REFERENCES users(id) ON DELETE CASCADE,"
                "UNIQUE (meeting_id, student_id),"
                "assigned_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                ");");

            txn.commit();
            std::cout << "Tables created successfully or already exist." << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error while creating tables: " << e.what() << std::endl;
            throw;
        }
    }

public:
    void initialize()
    {
        createDatabaseIfNotExists();
        createTablesIfNotExists();
    }
};

int main()
{
    try
    {
        DatabaseInitializer dbInit;
        dbInit.initialize();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Initialization failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
