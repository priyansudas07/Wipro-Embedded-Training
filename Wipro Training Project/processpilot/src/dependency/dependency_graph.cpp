#include "dependency_graph.hpp"
#include "logger.hpp"
#include <queue>

void DependencyGraph::buildGraph(const std::vector<ServiceConfig>& configs) {
    adj_list_.clear();
    nodes_.clear();

    for (const auto& cfg : configs) {
        nodes_.insert(cfg.name);
        for (const auto& dep : cfg.dependencies) {
            nodes_.insert(dep);
            // Edge from dependency -> service (dep must start first)
            adj_list_[dep].push_back(cfg.name);
        }
    }
}

std::vector<std::string> DependencyGraph::getStartOrder() {
    std::unordered_map<std::string, int> in_degree;
    for (const auto& node : nodes_) {
        in_degree[node] = 0;
    }

    for (const auto& pair : adj_list_) {
        for (const auto& neighbor : pair.second) {
            in_degree[neighbor]++;
        }
    }

    std::queue<std::string> q;
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            q.push(pair.first);
        }
    }

    std::vector<std::string> order;
    while (!q.empty()) {
        std::string curr = q.front();
        q.pop();
        order.push_back(curr);

        for (const auto& neighbor : adj_list_[curr]) {
            in_degree[neighbor]--;
            if (in_degree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    if (order.size() != nodes_.size()) {
        Logger::getInstance().log(LogLevel::ERROR, "Circular dependency detected in service graph!");
    }

    return order;
}

bool DependencyGraph::hasCycle() {
    auto order = getStartOrder();
    return order.size() != nodes_.size();
}
