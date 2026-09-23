#include <iostream>
#include "sys_info.hpp"

int main(int argc, char *argv[]) {
    SystemInfoTool tool;

    if (argc > 1) {
        int arg = std::stoi(argv[1]);
        tool.printBanner();
        if (arg == 1) tool.showSystemInfo();
        else if (arg == 2) tool.showCpuInfo();
        else if (arg == 3) tool.showMemoryInfo();
        else tool.showAllInfo();
        return 0;
    }

    int choice = 0;

    while (true) {
        tool.printBanner();
        std::cout << "  1. System Information (OS, Kernel, Uptime, Hostname)\n";
        std::cout << "  2. CPU Information (Model, Cores, Frequency)\n";
        std::cout << "  3. Memory Information (Total, Used, Free RAM & Swap)\n";
        std::cout << "  4. Display Full System Summary\n";
        std::cout << "  5. Exit Program\n";
        std::cout << "----------------------------------------------------------\n";
        std::cout << "Select an option (1-5): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid input! Please enter a number between 1 and 5.\n\n";
            continue;
        }

        switch (choice) {
            case 1:
                tool.showSystemInfo();
                break;
            case 2:
                tool.showCpuInfo();
                break;
            case 3:
                tool.showMemoryInfo();
                break;
            case 4:
                tool.showAllInfo();
                break;
            case 5:
                std::cout << "\nExiting Linux System Information Tool. Goodbye!\n";
                return 0;
            default:
                std::cout << "\nInvalid choice! Please select an option from 1 to 5.\n";
                break;
        }

        std::cout << "\nPress ENTER to return to main menu...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
        std::cout << "\n";
    }

    return 0;
}
