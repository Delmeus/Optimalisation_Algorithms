//
// Created by Antek on 17.10.2025.
//

#include "TabuSearch.hpp"
#include "Greedy.hpp"
#include "../utils/Timer.hpp"
#include <algorithm>
#include <climits>
#include <utility>
#include <random>
#include <ranges>

Solution TabuSearch::solve(int max_number_of_iterations, int id) {
    std::ofstream file;
    file.open("../../output/tabu/generations/" + instance.name  + "/gen" + std::to_string(id) + ".csv");
    auto bestSolution = Greedy::greedySolution(instance);
    auto currentSolution = bestSolution;

    Timer timer;
    timer.start();
    int iteration = 0;
    while(iteration < max_number_of_iterations) {
        auto neighbours = generateNeighbours(currentSolution);
        if (file.is_open()) {
            std::vector<Solution> solutions;
            solutions.reserve(neighbours.size());
            std::transform(neighbours.begin(), neighbours.end(),
                           std::back_inserter(solutions),
                           [](const Neighbour& n) { return n.solution; });
            utils::writeGenerationToFile(file, solutions);
        }

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
            if (!isMoveTabu(n.move) || n.solution.cost < bestSolution.cost) {
                currentSolution = n.solution;
                tabuList.emplace_back(n.move, instance.dimension / 2 + rand() % 10);
                break;
            }
        }

        if (currentSolution.cost < bestSolution.cost) {
            bestSolution = currentSolution;
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

    std::uniform_int_distribution<> randomCustomer(0, static_cast<int>(path.size() - 1));

    for (int i = 0; i < 500; ++i) {
        int c1 = randomCustomer(gen);
        int c2 = randomCustomer(gen);
        while (c1 == c2)
            c2 = randomCustomer(gen);

        if (std::ranges::find(usedCombinations, std::make_pair(c1, c2)) != usedCombinations.end() ||
            std::ranges::find(usedCombinations, std::make_pair(c2, c1)) != usedCombinations.end())
            continue;

        usedCombinations.emplace_back(c1, c2);

        std::vector<int> newPath = path;
        std::swap(newPath[c1], newPath[c2]);

        Neighbour n;
        int currentLoad = 0;
        std::vector<int> currentRoute;

        for (int customerId : newPath) {
            const auto& customer = instance.nodes[customerId - 1];

            if (currentLoad + customer.demand > instance.capacity) {
                n.solution.routes.push_back(currentRoute);
                currentRoute.clear();
                currentLoad = 0;
            }

            currentRoute.push_back(customerId);
            currentLoad += customer.demand;
        }

        if (!currentRoute.empty())
            n.solution.routes.push_back(currentRoute);

        n.solution.calculateAndSetCost(instance);
        neighbours.push_back(std::move(n));
    }
    std::ranges::sort(neighbours, [](const Neighbour& a, const Neighbour& b) {
        return a.solution.cost < b.solution.cost;
    });
    return neighbours;
}

bool TabuSearch::isMoveTabu(const Move& move) {
    auto it = std::find_if(tabuList.begin(), tabuList.end(), [&move](const std::pair<Move, int>& m) {
        return (m.first.customers.first == move.customers.first && m.first.customers.second == move.customers.second ||
                m.first.customers.first == move.customers.second && m.first.customers.second == move.customers.first);
    });
    return it != tabuList.end();
}


