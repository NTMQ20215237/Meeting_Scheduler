#include "Server.h"
#include <iostream>
#include <thread>

int main() {
    int port = 8079;
    Server server(port);

    std::cout << "Starting server..." << std::endl;
    server.start();
    // Server sẽ chạy liên tục, mở rộng điều kiện dừng server nếu cần thiết
    // server.stop(); // Thường dừng khi nhận tín hiệu hoặc điều kiện từ luồng khác
    // server.stop() có thể được thực hiện khi bạn thêm phương thức để dừng server qua tín hiệu ngắt.
    
    return 0;
}