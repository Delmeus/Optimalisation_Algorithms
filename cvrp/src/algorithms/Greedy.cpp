//
// Created by Antek on 14.10.2025.
//

#include "Greedy.hpp"
#include "../utils/common_utils.hpp"
#include <climits>


Solution Greedy::greedySolution(ProblemInstance& instance, int firstCustomer) {
    Solution solution;
    int depotId = instance.depots[0];
    const utils::Node& depot = instance.nodes[depotId - 1];

    std::vector<bool> visited(instance.dimension, false);
    visited[depotId - 1] = true;

    int remaining = instance.dimension - 1;

    while (remaining > 0) {
        std::vector<int> route;
        int routeCost = 0;
        int load = 0;
        const utils::Node* current = &depot;

        if (firstCustomer >= 1 && firstCustomer <= instance.dimension && firstCustomer != depotId &&
            !visited[firstCustomer - 1] && instance.nodes[firstCustomer - 1].demand <= instance.capacity) {

            const utils::Node& firstNode = instance.nodes[firstCustomer - 1];
            route.push_back(firstNode.id);
            routeCost += utils::euclidean_distance(depot, firstNode);
            load += firstNode.demand;
            visited[firstCustomer - 1] = true;
            remaining--;
            current = &firstNode;
        }

        while (true) {
            int bestDist = INT_MAX;
            int bestIdx = -1;

            for (const auto& node : instance.nodes) {
                if (node.id == depotId) continue;
                if (visited[node.id - 1]) continue;
                if (load + node.demand > instance.capacity) continue;

                int d = utils::euclidean_distance(*current, node);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx = node.id - 1;
                }
            }

            if (bestIdx == -1)
                break;

            visited[bestIdx] = true;
            route.push_back(instance.nodes[bestIdx].id);
            routeCost += bestDist;
            load += instance.nodes[bestIdx].demand;
            current = &instance.nodes[bestIdx];
            remaining--;
        }

        routeCost += utils::euclidean_distance(*current, depot);
        solution.routes.push_back(route);
        solution.cost += routeCost;

        firstCustomer = -1;
    }

    return solution;
}
