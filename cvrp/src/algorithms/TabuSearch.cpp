//
// Created by Antek on 17.10.2025.
//

#include "TabuSearch.hpp"
#include "Greedy.hpp"
#include "../utils/Timer.hpp"
#include <algorithm>
#include <climits>
#include <iostream>
#include <utility>
#include <random>
#include <ranges>
#include <iostream>

Solution TabuSearch::solve(int max_number_of_iterations, int id) {
    std::ofstream file;
    file.open("../../output/tabu/generations/gen" + std::to_string(id) + ".csv");
    auto bestSolution = Greedy::greedySolution(instance);
    auto currentSolution = bestSolution;
    bestSolution.cost = INT_MAX;

    Timer timer;
    timer.start();
    int iteration = 0;
    while(iteration < max_number_of_iterations) {
        auto neighbours = generateNeighbours(currentSolution);
        std::cout << "n size = " << neighbours.size() << std::endl;
        if (file.is_open()) {
            std::vector<Solution> solutions;
            solutions.reserve(neighbours.size());
            std::transform(neighbours.begin(), neighbours.end(),
                           std::back_inserter(solutions),
                           [](const Neighbour& n) { return n.solution; });
            utils::writeGenerationToFile(file, solutions);
        }
        std::cout << "yo" << std::endl;
        if (neighbours.empty()) {
            iteration++;
            for(auto& move : tabuList)
                move.second--;
            continue;
        }

        for (auto& n : neighbours) {
            if (n.solution.routes.empty()) {
                iteration++;
                for(auto& move : tabuList)
                    move.second--;
                continue;
            }
            if (n.solution.cost < bestSolution.cost) {
                bestSolution = n.solution;
            }
            if (!isMoveTabu(n.move)) {
                currentSolution = n.solution;
                tabuList.emplace_back(n.move, 100);
                break;
            }
        }

        iteration++;
        for (auto it = tabuList.begin(); it != tabuList.end();) {
            if (--(it->second) <= 0)
                it = tabuList.erase(it);
            else ++it;
        }
    }
    if (file.is_open())
        file.close();

    return bestSolution;
}

TabuSearch::TabuSearch(ProblemInstance instance) : instance{std::move(instance)}
{}

std::vector<Neighbour> TabuSearch::generateNeighbours(const Solution& solution) {
    std::vector<Neighbour> neighbours;
    std::vector<int> path;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::pair<int, int>> usedCombinations;

    for (const auto& route : solution.routes) {
        path.insert(path.end(), route.begin(), route.end());
    }

    std::uniform_int_distribution<> randomCustomer(0, static_cast<int>(solution.routes.size() - 1));

    for (int i = 0; i < 5 * instance.dimension; ++i) {
        int c1 = randomCustomer(gen);
        int c2 = randomCustomer(gen);
        while (c1 == c2)
            c2 = randomCustomer(gen);

        if (std::ranges::find(usedCombinations, std::make_pair(c1, c2)) != usedCombinations.end() ||
                std::ranges::find(usedCombinations, std::make_pair(c2, c1)) != usedCombinations.end())
            continue;

        std::vector<int> newPath = path;
        std::swap(newPath[c1], newPath[c2]);

        int currentLoad = 0;
        std::vector<int> currentRoute;
        Neighbour n;
        const utils::Node& depot = instance.nodes[instance.depots[0] - 1];
        const utils::Node* currentNode = &depot;
        for (int customerId : newPath) {
            const auto& customer = instance.nodes[customerId - 1];
            if (currentLoad + customer.demand > instance.capacity) {
                n.solution.routes.push_back(currentRoute);
                n.solution.cost += utils::euclidean_distance(*currentNode, depot);
                currentRoute.clear();
                currentLoad = 0;
                currentNode = &depot;
            }
            currentRoute.push_back(customerId);
            n.solution.cost += utils::euclidean_distance(*currentNode, customer);
            currentLoad += customer.demand;
            currentNode = &customer;
        }
        std::ranges::sort(neighbours, [](const Neighbour& a, const Neighbour& b) {
            return a.solution.cost < b.solution.cost;
        });
        neighbours.push_back(std::move(n));
    }

    return neighbours;
}

Neighbour TabuSearch::findBestNeighbour(const std::vector<Neighbour>& neighbours) {
    int bestCost = INT_MAX;
    Neighbour bestNeighbour;
    for (auto& n : neighbours) {
        if (n.solution.cost < bestCost && !isMoveTabu(n.move)) {
            bestCost = n.solution.cost;
            bestNeighbour = n;
        }
    }
    return bestNeighbour;
}

/*
std::vector<Neighbour> TabuSearch::generateNeighbours(const Solution& solution) {
    std::vector<Neighbour> neighbours;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> randomRoute(0, static_cast<int>(solution.routes.size() - 1));
    std::vector<std::pair<int, int>> usedCombinations;

    for (int r = 0; r < 5; ++r) {
        int r1 = randomRoute(gen);
        int r2 = randomRoute(gen);
        while (std::find(usedCombinations.begin(), usedCombinations.end(), std::make_pair(r1, r2)) != usedCombinations.end() ||
                std::find(usedCombinations.begin(), usedCombinations.end(), std::make_pair(r2, r1)) != usedCombinations.end())
            r2 = randomRoute(gen);
        for (size_t i = 0; i < solution.routes[r1].size(); ++i) {
            for (size_t j = 0; j < solution.routes[r2].size(); ++j) {
                if (r1 == r2 && i == j)
                    continue;

                Neighbour n;
                n.solution = solution;
                std::swap(n.solution.routes[r1][i], n.solution.routes[r2][j]);
                n.solution.calculateAndSetCost(instance);

                if (!n.solution.isValid(instance) || n.solution.routeExceedingCapacity(instance) != -1)
                    continue;

                n.move.customers = {solution.routes[r1][i], solution.routes[r2][j]};
                neighbours.push_back(std::move(n));
            }
        }
    }

    return neighbours;
}*/

bool TabuSearch::isMoveTabu(const Move& move) {
    auto it = std::find_if(tabuList.begin(), tabuList.end(), [&move](const std::pair<Move, int>& m) {
        return (m.first.customers.first == move.customers.first && m.first.customers.second == move.customers.second ||
                m.first.customers.first == move.customers.second && m.first.customers.second == move.customers.first);
    });
    return it != tabuList.end();
}


