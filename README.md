### REQUIREMENT: libpqxx (for Postgres database)
### RUN:
##### Must config all your datase paramaters in the all "Database" file: "DatabaseInitializer.cpp" and "DatabaseManager.cpp"

- Create database: 

```base
g++ -o DatabaseInitializer DatabaseInitializer.cpp -lpqxx -lpq
./DatabaseInitializer
```
- C_MAKE_FILE_LISTS:
```base
mkdir build
cd build
cmake ..
make
```
- Run and Test:
```base
./meeting_scheduler_server
```
Then, open a new terminal and run:
```base
./meeting_scheduler_client
```

TESTING:
```base
./meeting_scheduler_client
Enter command (REGISTER/LOGIN) or type EXIT to quit: LOGIN
Email: user@example.com
Password: user
Server response: 200;Login successful by teacher

Select an teacher's option:
1. Enter content (meeting minutes) for meetings
2. Logout
3. Exit
Enter your choice: 1
Enter content
Meeting id: 1
Server response: 200;Teacher and meeting match
Permission Accept
Enter content (enter 'END' to finish):
Hello
This is the third report for this project
I dont know exactly if it will be working correctly or not
END
Server response: 200;Content created successfully

Select an teacher's option:
1. Enter content (meeting minutes) for meetings
2. Logout
3. Exit
Enter your choice: 1   
Enter content
Meeting id: 1
Server response: 200;Teacher and meeting match
Permission Accept
Enter content (enter 'END' to finish):
Ayzo     I'm using the space for testing
Will it work?????
    KKKKKKK       Maybe it will not                            
END
Server response: 200;Content created successfully

Select an teacher's option:
1. Enter content (meeting minutes) for meetings
2. Logout
3. Exit
Enter your choice: ^C
```