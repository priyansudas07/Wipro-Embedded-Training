#ifndef PROCESS_MONITOR_HPP
#define PROCESS_MONITOR_HPP

#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
    std::string state;
    int ppid;
};

class ProcessMonitor {
public:
    ProcessMonitor() = default;
    void printBanner();
    bool readProcessInfo(int pid, ProcessInfo& info);
    void listAllProcesses();
    void searchProcess(const std::string& query);
    void showProcessStats();
};

#endif // PROCESS_MONITOR_HPP
