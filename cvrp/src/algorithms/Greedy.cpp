//
// Created by Antek on 14.10.2025.
//

#include "Greedy.hpp"
#include <climits>

Solution Greedy::greedySolution(ProblemInstance& instance, int firstCustomer) {
    Solution solution;
    int depotId = instance.depots[0];
    const utils::Node& depot = instance.nodes[depotId - 1];

    std::vector<bool> visited(instance.dimension, false);
    visited[depotId - 1] = true;
    std::vector<int> path;
    const utils::Node* current = &depot;

    if (firstCustomer >= 1 && firstCustomer <= instance.dimension && firstCustomer != depotId) {
        const utils::Node& firstNode = instance.nodes[firstCustomer - 1];
        path.push_back(firstNode.id);
        visited[firstCustomer - 1] = true;
        current = &firstNode;
    }

    while (path.size() < instance.dimension - 1) {
        int bestDist = INT_MAX;
        int bestIdx = -1;
        for (const auto& node : instance.nodes) {
            if (node.id == depotId)
                continue;
            if (visited[node.id - 1])
                continue;

            int d = utils::euclidean_distance(*current, node);
            if (d < bestDist) {
                bestDist = d;
                bestIdx = node.id - 1;
            }
        }

        if (bestIdx == -1)
            break;

        visited[bestIdx] = true;
        path.push_back(instance.nodes[bestIdx].id);
        current = &instance.nodes[bestIdx];
    }

    std::vector<int> currentRoute;
    int currentLoad = 0;
    for (int customerId : path) {
        const utils::Node& node = instance.nodes[customerId - 1];
        if (currentLoad + node.demand > instance.capacity) {
            solution.routes.push_back(currentRoute);
            currentRoute.clear();
            currentLoad = 0;
        }

        currentRoute.push_back(customerId);
        currentLoad += node.demand;
    }

    if (!currentRoute.empty()) {
        solution.routes.push_back(currentRoute);
    }

    solution.calculateAndSetCost(instance);
    return solution;
}