//
// Created by Antek on 14.10.2025.
//

#include "Solution.hpp"
#include "ProblemInstance.hpp"
#include <algorithm>
#include <iostream>
#include <filesystem>

bool Solution::isValid(const ProblemInstance &instance) {
    std::vector<bool> visited(instance.dimension, false);

    visited[instance.depots[0] - 1] = true;

    for (const auto& route : routes) {
        for (int id : route) {
            if (id < 1 || id > instance.dimension)
                return false;
            if (visited[id - 1])
                return false;
            visited[id - 1] = true;
        }
    }
    return std::find(visited.begin(), visited.end(), false) == visited.end();
}

void Solution::calculateAndSetCost(const ProblemInstance &instance) {
    cost = 0;
    const auto& depot = instance.nodes[instance.depots[0] - 1];
    for (const auto& route : routes) {
        if (route.empty()) continue;

        cost += utils::euclidean_distance(depot, instance.nodes[route.front() - 1]);

        for (size_t i = 0; i < route.size() - 1; ++i) {
            const auto& from = instance.nodes[route[i] - 1];
            const auto& to   = instance.nodes[route[i + 1] - 1];
            cost += utils::euclidean_distance(from, to);
        }

        cost += utils::euclidean_distance(instance.nodes[route.back() - 1], depot);
    }
}

void Solution::printCapacityPerRoute(const ProblemInstance &instance) {
    for(int i = 1; i < routes.size() + 1; i++) {
        int capacity = 0;
        for (int stop : routes[i - 1]) {
            capacity += instance.nodes[stop - 1].demand;
        }
        std::cout << "Route #" << i << " capacity = " << capacity << std::endl;
    }
}

int Solution::routeExceedingCapacity(const ProblemInstance &instance) {
    for(int i = 1; i < routes.size() + 1; i++) {
        int capacity = 0;
        for (int stop : routes[i - 1]) {
            capacity += instance.nodes[stop - 1].demand;
        }
        if (capacity > instance.capacity)
            return i - 1;
    }
    return -1;
}

void Solution::saveFullSolutionToFile(const ProblemInstance &instance, const std::string& filename) {
    std::ofstream file;
    file.open(R"(G:\Projekty_Studia\_magisterka\algorytmy_optymalizacji\cvrp\output\)" + filename);
    if (file.is_open()) {
        file << "Cost," << cost << std::endl;
        for(int i = 1; i < routes.size() + 1; i++) {
            int capacity = 0;
            file << "Route#" << i << ",";
            for (int stop : routes[i - 1]) {
                capacity += instance.nodes[stop - 1].demand;
                file << stop << ",";
            }
            file << "Route_capacity," << capacity << std::endl;
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}

void Solution::saveSolutionToFile(const std::string &filename) {
    bool firstWrite = false;
    std::ofstream file;
    std::string name = R"(G:\Projekty_Studia\_magisterka\algorytmy_optymalizacji\cvrp\output\)" + filename;
    if (!std::filesystem::exists(name))
        firstWrite = true;

    file.open(name, std::ios_base::app);
    if (file.is_open()) {
        if (firstWrite)
            file << "Cost" << std::endl;
        file << cost << std::endl;
        file.close();
    }
    else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}
