#include <iostream>
#include "process_monitor.hpp"

int main(int argc, char *argv[]) {
    ProcessMonitor monitor;

    if (argc > 1) {
        int arg = std::stoi(argv[1]);
        monitor.printBanner();
        if (arg == 1) monitor.listAllProcesses();
        else if (arg == 2 && argc > 2) monitor.searchProcess(argv[2]);
        else monitor.showProcessStats();
        return 0;
    }

    int choice = 0;
    std::string query;

    while (true) {
        monitor.printBanner();
        std::cout << "  1. Show Process Details (PID, Name, State, PPID)\n";
        std::cout << "  2. Search Process by Name or PID\n";
        std::cout << "  3. View Process Statistics Summary\n";
        std::cout << "  4. Exit Program\n";
        std::cout << "----------------------------------------------------------------------\n";
        std::cout << "Select an option (1-4): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid choice! Please enter a number between 1 and 4.\n\n";
            continue;
        }

        switch (choice) {
            case 1:
                std::cout << "\n";
                monitor.listAllProcesses();
                break;
            case 2:
                std::cout << "\nEnter process name or PID to search: ";
                std::cin >> query;
                monitor.searchProcess(query);
                break;
            case 3:
                monitor.showProcessStats();
                break;
            case 4:
                std::cout << "\nExiting Linux Process Monitor. Goodbye!\n";
                return 0;
            default:
                std::cout << "\nInvalid choice! Please select an option between 1 and 4.\n";
                break;
        }

        std::cout << "\nPress ENTER to return to main menu...";
        std::cin.ignore(10000, '\n');
        std::cin.get();
        std::cout << "\n";
    }

    return 0;
}
