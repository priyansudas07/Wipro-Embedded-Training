#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

// Check whether a string contains only digits
bool isNumber(const string& str)
{
    if (str.empty())
        return false;

    for (char ch : str)
    {
        if (!isdigit(ch))
            return false;
    }

    return true;
}

// Get process name from /proc/PID/comm
string getProcessName(const string& pid)
{
    string path = "/proc/" + pid + "/comm";

    ifstream file(path);

    if (!file)
        return "Unknown";

    string name;
    getline(file, name);

    return name;
}

// Show running processes
void showRunningProcesses()
{
    cout << "\n====================================\n";
    cout << "        RUNNING PROCESSES\n";
    cout << "====================================\n";

    cout << "PID\tPROCESS\n";
    cout << "-----------------------------\n";

    int count = 0;

    try
    {
        for (const auto& entry : fs::directory_iterator("/proc"))
        {
            string dirname = entry.path().filename().string();

            if (isNumber(dirname))
            {
                string processName = getProcessName(dirname);

                cout << dirname << "\t" << processName << endl;

                count++;

                // Display first 30 processes
                if (count >= 30)
                    break;
            }
        }
    }
    catch (const exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }
}

// Show details of a specific process
void showProcessDetails()
{
    string pid;

    cout << "\nEnter PID: ";
    cin >> pid;

    if (!isNumber(pid))
    {
        cout << "Invalid PID.\n";
        return;
    }

    string path = "/proc/" + pid + "/status";

    ifstream file(path);

    if (!file)
    {
        cout << "Process not found.\n";
        return;
    }

    string line;

    cout << "\n===== PROCESS DETAILS =====\n";

    while (getline(file, line))
    {
        if (line.find("Name:") == 0 ||
            line.find("State:") == 0 ||
            line.find("Pid:") == 0 ||
            line.find("PPid:") == 0)
        {
            cout << line << endl;
        }
    }
}

// Count running processes
void countProcesses()
{
    int count = 0;

    try
    {
        for (const auto& entry : fs::directory_iterator("/proc"))
        {
            string dirname = entry.path().filename().string();

            if (isNumber(dirname))
            {
                count++;
            }
        }
    }
    catch (const exception& e)
    {
        cout << "Error: " << e.what() << endl;
        return;
    }

    cout << "\nTotal Processes: " << count << endl;
}

// Search process by PID
void searchProcess()
{
    string pid;

    cout << "\nEnter PID to search: ";
    cin >> pid;

    if (!isNumber(pid))
    {
        cout << "Invalid PID.\n";
        return;
    }

    string path = "/proc/" + pid;

    if (fs::exists(path))
    {
        cout << "\nProcess found!\n";
        cout << "PID  : " << pid << endl;
        cout << "Name : " << getProcessName(pid) << endl;
    }
    else
    {
        cout << "\nProcess not found.\n";
    }
}

// Generate process report
void generateReport()
{
    ofstream report("process_report.txt");

    if (!report)
    {
        cout << "Unable to create report.\n";
        return;
    }

    report << "====================================\n";
    report << "        LINUX PROCESS REPORT\n";
    report << "====================================\n\n";

    report << "PID\tPROCESS\n";
    report << "-----------------------------\n";

    int count = 0;

    try
    {
        for (const auto& entry : fs::directory_iterator("/proc"))
        {
            string dirname = entry.path().filename().string();

            if (isNumber(dirname))
            {
                string processName = getProcessName(dirname);
                report << dirname << "\t" << processName << "\n";
                count++;
            }
        }
    }
    catch (const exception& e)
    {
        cout << "Error: " << e.what() << endl;
        return;
    }

    report << "-----------------------------\n";
    report << "Total Processes Logged: " << count << "\n";
    report.close();

    cout << "\nProcess report generated successfully in 'process_report.txt'!\n";
}

int main(int argc, char *argv[])
{
    // Non-interactive batch execution support for automated testing/logs
    if (argc > 1)
    {
        int arg = atoi(argv[1]);
        if (arg == 1) showRunningProcesses();
        else if (arg == 3) countProcesses();
        else if (arg == 5) generateReport();
        return 0;
    }

    int choice = 0;

    while (true)
    {
        cout << "\n====================================\n";
        cout << "   LINUX SYSTEM RESOURCE MONITOR\n";
        cout << "====================================\n";
        cout << "1. Show Running Processes (Top 30)\n";
        cout << "2. Show Process Details by PID\n";
        cout << "3. Count Total Processes\n";
        cout << "4. Search Process by PID\n";
        cout << "5. Generate Process Report (process_report.txt)\n";
        cout << "6. Exit\n";
        cout << "------------------------------------\n";
        cout << "Enter your choice (1-6): ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                showRunningProcesses();
                break;
            case 2:
                showProcessDetails();
                break;
            case 3:
                countProcesses();
                break;
            case 4:
                searchProcess();
                break;
            case 5:
                generateReport();
                break;
            case 6:
                cout << "\nExiting System Resource Monitor. Goodbye!\n";
                return 0;
            default:
                cout << "\nInvalid choice. Please select an option from 1 to 6.\n";
                break;
        }
    }

    return 0;
}
