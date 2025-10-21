//
// Created by Antek on 14.10.2025.
//

#include "Greedy.hpp"
#include "../utils/common_utils.hpp"
#include <climits>


Solution Greedy::greedySolution(ProblemInstance& instance) {
    Solution solution;
    int depotId = instance.depots[0];
    const utils::Node depot = instance.nodes[depotId - 1];

    std::vector<bool> visited(instance.dimension, false);
    visited[depotId - 1] = true;

    int remaining = instance.dimension - 1;

    while (remaining > 0) {
        std::vector<int> route;
        int cost = 0;
        int load = 0;
        const utils::Node* current = &depot;

        while (true) {
            int bestDist = INT_MAX;
            int bestIdx = -1;

            for (const auto& node : instance.nodes) {
                if (node.id == depotId)
                    continue;
                if (visited[node.id - 1])
                    continue;
                if (load + node.demand > instance.capacity)
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
            route.push_back(instance.nodes[bestIdx].id);
            cost += bestDist;
            load += instance.nodes[bestIdx].demand;
            current = &instance.nodes[bestIdx];
            remaining--;
        }

        cost += euclidean_distance(*current, depot);
        solution.routes.push_back(route);
        solution.cost += cost;
    }

    return solution;
}
