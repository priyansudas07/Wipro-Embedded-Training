#include "sys_info.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <unistd.h>

void SystemInfoTool::printBanner() {
    std::cout << "==========================================================\n";
    std::cout << "        LINUX SYSTEM INFORMATION TOOL (Wipro COE)         \n";
    std::cout << "==========================================================\n";
}

void SystemInfoTool::printSectionHeader(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

void SystemInfoTool::showSystemInfo() {
    printSectionHeader("1. SYSTEM INFORMATION");

    struct utsname sys_uts;
    if (uname(&sys_uts) == 0) {
        std::cout << "  " << std::left << std::setw(20) << "OS Name" << " : " << sys_uts.sysname << "\n";
        std::cout << "  " << std::left << std::setw(20) << "Hostname" << " : " << sys_uts.nodename << "\n";
        std::cout << "  " << std::left << std::setw(20) << "Kernel Release" << " : " << sys_uts.release << "\n";
        std::cout << "  " << std::left << std::setw(20) << "Kernel Version" << " : " << sys_uts.version << "\n";
        std::cout << "  " << std::left << std::setw(20) << "Architecture" << " : " << sys_uts.machine << "\n";
    }

    std::ifstream os_file("/etc/os-release");
    if (os_file.is_open()) {
        std::string line;
        while (std::getline(os_file, line)) {
            if (line.rfind("PRETTY_NAME=", 0) == 0) {
                std::string name = line.substr(12);
                if (!name.empty() && name.front() == '"') name.erase(0, 1);
                if (!name.empty() && name.back() == '"') name.pop_back();
                std::cout << "  " << std::left << std::setw(20) << "OS Distribution" << " : " << name << "\n";
                break;
            }
        }
        os_file.close();
    }

    struct sysinfo s_info;
    if (sysinfo(&s_info) == 0) {
        long uptime_sec = s_info.uptime;
        long days = uptime_sec / (24 * 3600);
        uptime_sec %= (24 * 3600);
        long hours = uptime_sec / 3600;
        uptime_sec %= 3600;
        long minutes = uptime_sec / 60;
        long seconds = uptime_sec % 60;

        std::cout << "  " << std::left << std::setw(20) << "System Uptime" << " : "
                  << days << " days, " << hours << " hrs, " << minutes << " mins, " << seconds << " secs\n";
    }
}

void SystemInfoTool::showCpuInfo() {
    printSectionHeader("2. CPU INFORMATION");

    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    long total_conf_cores = sysconf(_SC_NPROCESSORS_CONF);
    std::cout << "  " << std::left << std::setw(20) << "CPU Cores" << " : "
              << num_cores << " Active Cores (" << total_conf_cores << " Configured)\n";

    std::ifstream cpu_file("/proc/cpuinfo");
    if (cpu_file.is_open()) {
        std::string line;
        std::string model_name = "Unknown CPU Model";
        std::string cpu_mhz = "Unknown";
        std::string cache_size = "Unknown";
        bool found_model = false;

        while (std::getline(cpu_file, line)) {
            if (!found_model && line.rfind("model name", 0) == 0) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    model_name = line.substr(colon + 2);
                    found_model = true;
                }
            } else if (line.rfind("cpu MHz", 0) == 0) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    cpu_mhz = line.substr(colon + 2);
                }
            } else if (line.rfind("cache size", 0) == 0) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    cache_size = line.substr(colon + 2);
                }
            }
        }
        cpu_file.close();

        std::cout << "  " << std::left << std::setw(20) << "Model Name" << " : " << model_name << "\n";
        if (cpu_mhz != "Unknown") {
            std::cout << "  " << std::left << std::setw(20) << "CPU Frequency" << " : " << cpu_mhz << " MHz\n";
        }
        if (cache_size != "Unknown") {
            std::cout << "  " << std::left << std::setw(20) << "Cache Size" << " : " << cache_size << "\n";
        }
    }
}

void SystemInfoTool::showMemoryInfo() {
    printSectionHeader("3. MEMORY INFORMATION");

    struct sysinfo s_info;
    if (sysinfo(&s_info) == 0) {
        double unit_gb = 1024.0 * 1024.0 * 1024.0;
        double total_ram_gb = (double)s_info.totalram * s_info.mem_unit / unit_gb;
        double free_ram_gb  = (double)s_info.freeram  * s_info.mem_unit / unit_gb;
        double buffer_ram_gb = (double)s_info.bufferram * s_info.mem_unit / unit_gb;
        double used_ram_gb  = total_ram_gb - free_ram_gb - buffer_ram_gb;
        double used_percent = (used_ram_gb / total_ram_gb) * 100.0;

        double total_swap_gb = (double)s_info.totalswap * s_info.mem_unit / unit_gb;
        double free_swap_gb  = (double)s_info.freeswap  * s_info.mem_unit / unit_gb;
        double used_swap_gb  = total_swap_gb - free_swap_gb;

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  " << std::left << std::setw(20) << "Total RAM" << " : " << total_ram_gb << " GB\n";
        std::cout << "  " << std::left << std::setw(20) << "Used RAM" << " : " << used_ram_gb << " GB (" << std::setprecision(1) << used_percent << "%)\n";
        std::cout << std::setprecision(2);
        std::cout << "  " << std::left << std::setw(20) << "Free RAM" << " : " << free_ram_gb << " GB\n";
        std::cout << "  " << std::left << std::setw(20) << "Buffer / Cache" << " : " << buffer_ram_gb << " GB\n";
        std::cout << "  " << std::left << std::setw(20) << "Swap Memory" << " : " << total_swap_gb << " GB (Used: " << used_swap_gb << " GB, Free: " << free_swap_gb << " GB)\n";
    }
}

void SystemInfoTool::showAllInfo() {
    showSystemInfo();
    showCpuInfo();
    showMemoryInfo();
}
