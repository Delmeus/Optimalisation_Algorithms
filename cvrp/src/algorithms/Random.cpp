//
// Created by Antek on 14.10.2025.
//

#include "Random.hpp"
#include <algorithm>
#include <random>

Solution Random::randomSolution(const ProblemInstance &instance) {
    Solution solution;
    solution.routes.clear();
    solution.cost = 0;

    const utils::Node& depot = instance.nodes[instance.depots[0] - 1];

    std::vector<int> customers;
    customers.reserve(instance.dimension - instance.depots.size());
    for (const auto& node : instance.nodes) {
        if (std::find(instance.depots.begin(), instance.depots.end(), node.id) == instance.depots.end())
            customers.push_back(node.id);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(customers.begin(), customers.end(), gen);

    std::vector<int> currentRoute;
    int currentLoad = 0;
    const utils::Node* currentNode = &depot;

    for (int customerId : customers) {
        const auto& customer = instance.nodes[customerId - 1];
        if (currentLoad + customer.demand > instance.capacity) {
            solution.routes.push_back(currentRoute);
            solution.cost += utils::euclidean_distance(*currentNode, depot);
            currentRoute.clear();
            currentLoad = 0;
            currentNode = &depot;
        }
        currentRoute.push_back(customerId);
        solution.cost += utils::euclidean_distance(*currentNode, customer);
        currentLoad += customer.demand;
        currentNode = &customer;
    }

    if (!currentRoute.empty()) {
        solution.routes.push_back(currentRoute);
        solution.cost += utils::euclidean_distance(*currentNode, depot);
    }

    return solution;
}
