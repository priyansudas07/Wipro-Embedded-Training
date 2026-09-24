#include <iostream>
#include <string>
#include "unix_socket.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: processpilot_cli <status|start|stop|restart> [service_name]" << std::endl;
        return 1;
    }

    std::string cmd = argv[1];
    if (argc > 2) {
        cmd += " ";
        cmd += argv[2];
    }

    std::string response = UnixSocketClient::sendCommand(cmd);
    std::cout << response << std::endl;

    return 0;
}
