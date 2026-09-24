#ifndef DAEMON_HPP
#define DAEMON_HPP

#include <string>
#include "process_manager.hpp"
#include "unix_socket.hpp"

class ProcessPilotDaemon {
public:
    ProcessPilotDaemon() = default;
    bool initialize(const std::string& config_file);
    void runSingleLoop();
    ProcessManager& getProcessManager() { return pm_; }

private:
    ProcessManager pm_;
    UnixSocketServer ipc_server_;
};

#endif // DAEMON_HPP
