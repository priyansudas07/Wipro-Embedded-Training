#include "unix_socket.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

UnixSocketServer::~UnixSocketServer() {
    if (server_fd_ >= 0) close(server_fd_);
    if (!socket_path_.empty()) unlink(socket_path_.c_str());
}

bool UnixSocketServer::init(const std::string& socket_path) {
    socket_path_ = socket_path;
    unlink(socket_path_.c_str());

    server_fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd_ < 0) return false;

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path_.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) return false;
    if (listen(server_fd_, 5) < 0) return false;

    return true;
}

std::string UnixSocketClient::sendCommand(const std::string& command, const std::string& socket_path) {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) return "Error: Failed to create client socket";

    struct sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socket_path.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(client_fd);
        return "Error: Could not connect to ProcessPilot daemon at " + socket_path;
    }

    send(client_fd, command.c_str(), command.size(), 0);

    char buffer[4096];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    close(client_fd);
    if (bytes > 0) return std::string(buffer);
    return "Error: Empty response from daemon";
}
