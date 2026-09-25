#include "sandboxx/utils.hpp"
#include <iostream>
#include <string>
#include <vector>

struct CLICommand {
    std::string action;
    std::string config_file;
    std::string sandbox_id;
    std::string exec_command;
};

class CommandParser {
public:
    static CLICommand parse(int argc, char* argv[]) {
        CLICommand cmd;
        if (argc < 2) {
            cmd.action = "help";
            return cmd;
        }

        cmd.action = argv[1];

        if (cmd.action == "run") {
            if (argc > 2) cmd.config_file = argv[2];
            if (argc > 3) {
                for (int i = 3; i < argc; ++i) {
                    cmd.exec_command += std::string(argv[i]) + " ";
                }
            }
        } else if (cmd.action == "stop" || cmd.action == "inspect") {
            if (argc > 2) cmd.sandbox_id = argv[2];
        }

        return cmd;
    }

    static void printUsage() {
        std::cout << "======================================================\n";
        std::cout << "  SandBoxX — Linux Process Isolation System           \n";
        std::cout << "======================================================\n";
        std::cout << "Usage:\n";
        std::cout << "  sandboxx run <config.json> [command]   Run process inside sandbox\n";
        std::cout << "  sandboxx list                          List active sandboxes\n";
        std::cout << "  sandboxx inspect <sandbox_id>          Inspect sandbox details\n";
        std::cout << "  sandboxx stop <sandbox_id>             Stop running sandbox\n";
        std::cout << "  sandboxx help                          Show usage guidance\n";
        std::cout << "------------------------------------------------------\n";
    }
};
