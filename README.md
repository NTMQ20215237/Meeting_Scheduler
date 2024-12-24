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

Attention: 
- REGISTER (name: 'cannot handle spaces')
- Switching between functions sometimes causes errors, freezes, and requires reconnection from the client (./meeting_scheduler_client) 