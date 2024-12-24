cmake_minimum_required(VERSION 3.10)
project(MeetingScheduler)

# Chỉnh chuẩn C++ để sử dụng
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tìm kiếm các gói yêu cầu
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Thêm đường dẫn tới thư viện
include_directories(${PQXX_INCLUDE_DIRS})
link_directories(${PQXX_LIBRARY_DIRS})
add_definitions(${PQXX_CFLAGS_OTHER})

# Thêm các thư mục chứa file header
include_directories(include)


# Cấu hình tương tự cho server và client
set(SERVER_SOURCES
    src/Server/Server_main.cpp
    src/Server/Server.cpp
    src/Database/DatabaseManager.cpp
)

add_executable(meeting_scheduler_server ${SERVER_SOURCES})
target_link_libraries(meeting_scheduler_server ${PQXX_LIBRARIES})

set(CLIENT_SOURCES
    src/Client/Client_main.cpp
    src/Client/Client.cpp
)

add_executable(meeting_scheduler_client ${CLIENT_SOURCES})