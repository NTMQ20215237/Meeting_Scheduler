#include "MessageHandler.h"

std::string MessageHandler::handleRequest(const std::string& request) {
    // Tách command và parameter từ request
    // Ví dụ: LOGIN email password
    // Tạo phản hồi theo định dạng <response_code>;<message>
    if (request.find("LOGIN") == 0) {
        return "200;Login successful";
    }

    return "400;Unknown command";
}
