#include "dependency_graph.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::vector<ServiceConfig> configs;
    ServiceConfig s1; s1.name = "logger";
    ServiceConfig s2; s2.name = "web"; s2.dependencies.push_back("logger");

    configs.push_back(s1);
    configs.push_back(s2);

    DependencyGraph graph;
    graph.buildGraph(configs);

    auto order = graph.getStartOrder();
    assert(order.size() == 2);
    assert(order[0] == "logger");
    assert(order[1] == "web");

    std::cout << "[TEST DEPENDENCY] Topological order: PASSED" << std::endl;
    return 0;
}
