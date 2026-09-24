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

void ProcessPilotDaemon::runSingleLoop() {
    pm_.monitorProcesses();
}
