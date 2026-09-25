#include "command_parser.cpp"
#include "sandboxx/runtime.hpp"
#include "sandboxx/logger.hpp"
#include "sandboxx/utils.hpp"
#include <iostream>
#include <iomanip>

int runCLI(int argc, char* argv[]) {
    CLICommand cmd = CommandParser::parse(argc, argv);

    if (cmd.action == "help" || cmd.action == "--help" || cmd.action == "-h") {
        CommandParser::printUsage();
        return 0;
    }

    if (cmd.action == "run") {
        if (cmd.config_file.empty()) {
            std::cout << "Error: Config file required. Example: sandboxx run configs/default.json 'echo Hello'" << std::endl;
            return 1;
        }

        SandboxConfig config;
        if (!Utils::parseConfigFile(cmd.config_file, config)) {
            std::cout << "Error loading configuration: " << cmd.config_file << std::endl;
            return 1;
        }

        if (!cmd.exec_command.empty()) {
            config.command = cmd.exec_command;
        }

        SandboxRuntime runtime;
        bool ok = runtime.run(config);
        return ok ? 0 : 1;
    }

    if (cmd.action == "list") {
        auto sandboxes = SandboxRuntime::listSandboxes();
        std::cout << "======================================================\n";
        std::cout << "            ACTIVE SANDBOXX INSTANCES                 \n";
        std::cout << "======================================================\n";
        std::cout << std::left << std::setw(30) << "SANDBOX ID"
                  << std::setw(16) << "NAME"
                  << std::setw(8)  << "PID"
                  << std::setw(12) << "STATUS" << "\n";
        std::cout << "------------------------------------------------------\n";
        if (sandboxes.empty()) {
            std::cout << "No active sandboxes found.\n";
        } else {
            for (const auto& sb : sandboxes) {
                std::cout << std::left << std::setw(30) << sb.id
                          << std::setw(16) << sb.name
                          << std::setw(8)  << sb.pid
                          << std::setw(12) << sb.status << "\n";
            }
        }
        std::cout << "------------------------------------------------------\n";
        return 0;
    }

    if (cmd.action == "inspect") {
        if (cmd.sandbox_id.empty()) {
            std::cout << "Error: Sandbox ID required for inspect." << std::endl;
            return 1;
        }
        return SandboxRuntime::inspectSandbox(cmd.sandbox_id) ? 0 : 1;
    }

    if (cmd.action == "stop") {
        if (cmd.sandbox_id.empty()) {
            std::cout << "Error: Sandbox ID required for stop." << std::endl;
            return 1;
        }
        return SandboxRuntime::stopSandbox(cmd.sandbox_id) ? 0 : 1;
    }

    std::cout << "Unknown command: " << cmd.action << std::endl;
    CommandParser::printUsage();
    return 1;
}
