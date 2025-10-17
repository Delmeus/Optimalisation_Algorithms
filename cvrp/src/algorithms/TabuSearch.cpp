//
// Created by Antek on 17.10.2025.
//

#include "TabuSearch.hpp"
#include "Greedy.hpp"
#include "../utils/Timer.hpp"
#include <utility>
#include <random>

Solution TabuSearch::solve(int max_number_of_iterations) {
    auto bestSolution = Greedy::greedySolution(instance);
    auto currentSolution = bestSolution;
    bestSolution.cost = INT_MAX;

    Timer timer;
    timer.start();
    int iteration = 0;
    while(iteration < max_number_of_iterations) {
        auto neighbours = generateNeighbours(currentSolution);

        if (neighbours.empty()) {
            iteration++;
            for(auto& move : tabuList)
                move.second--;
            continue;
        }

        auto bestNeighbour = findBestNeighbour(neighbours);

        if (bestNeighbour.solution.routes.empty()) {
            iteration++;
            for(auto& move : tabuList)
                move.second--;
            continue;
        }

        tabuList.emplace_back(bestNeighbour.move, 100);
        currentSolution = bestNeighbour.solution;

        if (currentSolution.cost < bestSolution.cost) {
            bestSolution = currentSolution;
            timer.stop();
        }

        iteration++;
        for (auto it = tabuList.begin(); it != tabuList.end();) {
            if (--(it->second) <= 0)
                it = tabuList.erase(it);
            else ++it;
        }
    }
    return bestSolution;
}

TabuSearch::TabuSearch(ProblemInstance instance) : instance{std::move(instance)}
{}

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

bool TabuSearch::isMoveTabu(const Move& move) {
    auto it = std::find_if(tabuList.begin(), tabuList.end(), [&move](const std::pair<Move, int>& m) {
        return (m.first.customers.first == move.customers.first && m.first.customers.second == move.customers.second ||
                m.first.customers.first == move.customers.second && m.first.customers.second == move.customers.first);
    });
    return it != tabuList.end();
}


