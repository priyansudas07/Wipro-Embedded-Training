#include "daemon.hpp"
#include "config_parser.hpp"
#include "dependency_graph.hpp"
#include "logger.hpp"
#include <iostream>

bool ProcessPilotDaemon::initialize(const std::string& config_file) {
    Logger::getInstance().init("processpilot.log");
    Logger::getInstance().log(LogLevel::INFO, "Initializing ProcessPilot Supervisor Daemon...");

    auto service_configs = ConfigParser::parseFile(config_file);
    if (service_configs.empty()) {
        Logger::getInstance().log(LogLevel::WARNING, "No services parsed from config.");
        return false;
    }

    DependencyGraph graph;
    graph.buildGraph(service_configs);
    auto start_order = graph.getStartOrder();

    for (const auto& cfg : service_configs) {
        pm_.addService(cfg);
    }

    Logger::getInstance().log(LogLevel::INFO, "Starting managed services in dependency topological order...");
    for (const auto& name : start_order) {
        pm_.startService(name);
    }

    ipc_server_.init("/tmp/processpilot.sock");
    return true;
}

#include <sstream>
#include <cstdio>

void ProcessPilotDaemon::runSingleLoop() {
    pm_.monitorProcesses();

    ipc_server_.processPendingRequests([this](const std::string& cmd) -> std::string {
        std::istringstream iss(cmd);
        std::string action, name;
        iss >> action >> name;

        if (action == "list" || action == "LIST") {
            std::string result = "MANAGED SERVICES STATUS:\n";
            result += "--------------------------------------------------------\n";
            result += "NAME             STATUS       PID       RESTARTS\n";
            result += "--------------------------------------------------------\n";
            for (const auto& [svc_name, proc] : pm_.getAllServices()) {
                std::string st = (proc.status == ServiceStatus::RUNNING) ? "RUNNING" :
                                 (proc.status == ServiceStatus::FAILED) ? "FAILED" : "STOPPED";
                char line[256];
                snprintf(line, sizeof(line), "%-16s %-12s %-9d %d\n",
                         svc_name.c_str(), st.c_str(), (int)proc.pid, proc.restart_count);
                result += line;
            }
            return result;
        } else if (action == "status" || action == "STATUS") {
            if (name.empty()) return "Error: Service name required for status";
            auto* proc = pm_.getService(name);
            if (!proc) return "Error: Unknown service: " + name;
            std::string st = (proc->status == ServiceStatus::RUNNING) ? "RUNNING" :
                             (proc->status == ServiceStatus::FAILED) ? "FAILED" : "STOPPED";
            return "Service: " + name + "\nStatus: " + st + "\nPID: " + std::to_string(proc->pid) +
                   "\nRestarts: " + std::to_string(proc->restart_count);
        } else if (action == "start" || action == "START") {
            if (name.empty()) return "Error: Service name required for start";
            if (pm_.startService(name)) return "Successfully started service: " + name;
            return "Error: Failed to start service: " + name;
        } else if (action == "stop" || action == "STOP") {
            if (name.empty()) return "Error: Service name required for stop";
            if (pm_.stopService(name)) return "Successfully stopped service: " + name;
            return "Error: Failed to stop service: " + name;
        } else if (action == "restart" || action == "RESTART") {
            if (name.empty()) return "Error: Service name required for restart";
            if (pm_.restartService(name)) return "Successfully restarted service: " + name;
            return "Error: Failed to restart service: " + name;
        }
        return "Unknown command: " + cmd;
    });
}
