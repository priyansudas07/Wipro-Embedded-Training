#ifndef SYS_INFO_HPP
#define SYS_INFO_HPP

#include <string>

class SystemInfoTool {
public:
    SystemInfoTool() = default;
    void printBanner();
    void printSectionHeader(const std::string& title);
    void showSystemInfo();
    void showCpuInfo();
    void showMemoryInfo();
    void showAllInfo();
};

#endif // SYS_INFO_HPP
