#ifndef DEPENDENCY_GRAPH_HPP
#define DEPENDENCY_GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "config_parser.hpp"

class DependencyGraph {
public:
    DependencyGraph() = default;
    void buildGraph(const std::vector<ServiceConfig>& configs);
    std::vector<std::string> getStartOrder();
    bool hasCycle();

private:
    std::unordered_map<std::string, std::vector<std::string>> adj_list_;
    std::unordered_set<std::string> nodes_;
};

#endif // DEPENDENCY_GRAPH_HPP
