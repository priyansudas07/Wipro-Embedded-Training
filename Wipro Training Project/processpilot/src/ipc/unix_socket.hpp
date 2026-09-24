#ifndef UNIX_SOCKET_HPP
#define UNIX_SOCKET_HPP

#include <string>

class UnixSocketServer {
public:
    UnixSocketServer() = default;
    ~UnixSocketServer();
    bool init(const std::string& socket_path = "/tmp/processpilot.sock");
    std::string receiveCommand();
    void sendResponse(int client_fd, const std::string& response);

private:
    int server_fd_ = -1;
    std::string socket_path_;
};

class UnixSocketClient {
public:
    UnixSocketClient() = default;
    static std::string sendCommand(const std::string& command, const std::string& socket_path = "/tmp/processpilot.sock");
};

#endif // UNIX_SOCKET_HPP
