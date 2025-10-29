//
// Created by Antek on 14.10.2025.
//

#include "Genetic.hpp"
#include "Greedy.hpp"
#include "Random.hpp"
#include "../utils/Timer.hpp"
#include <algorithm>
#include <chrono>
#include <climits>
#include <iostream>
#include <random>
#include <unordered_set>
#include <utility>
#include <ranges>

constexpr double MINIMAL_REQUIRED_FITNESS = 0.7;
constexpr int MIN_NUMBER_OF_INDIVIDUALS = 20;
constexpr double ALLOW_INTO_NEXT_GENERATION_THRESHOLD = 0.97;

std::random_device rd;
std::mt19937 gen(rd());

Solution Genetic::solve(size_t max_population_size, int max_number_of_generations, int id) {
    std::ofstream file;
    file.open("../../output/genetic/generations/" + instance.name + "/gen" +  std::to_string(id) + ".csv");

    std::vector<Solution> population;
    population.push_back(Greedy::greedySolution(instance));
    while(population.size() < max_population_size)
        population.push_back(Random::randomSolution(instance));

    Solution bestSolution;
    bestSolution.cost = INT_MAX;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    int iteration = 0;
    while(iteration < max_number_of_generations){
        sortAndCalculateFitness(population);
        if(bestSolution.cost > population[0].cost)
            bestSolution = population[0];

        while ((population.size() > MIN_NUMBER_OF_INDIVIDUALS && population.back().fitness < MINIMAL_REQUIRED_FITNESS) ||
                population.size() > max_population_size)
            population.pop_back();

        if (file.is_open())
            utils::writeGenerationToFile(file, population);

        std::vector<Solution> nextGeneration;
        size_t eliteSolutions = 0;
        for(auto& element : population){
            if (eliteSolutions < max_population_size / 10 && element.fitness > ALLOW_INTO_NEXT_GENERATION_THRESHOLD && !containsSolution(nextGeneration, element)) {
                nextGeneration.push_back(element);
                eliteSolutions++;
            }

            double value = distribution(gen);
            if(value <= mutation_factor){
                Solution mutatedSolution;
                if (distribution(gen) > 0.1)
                    mutatedSolution = mutate(element, instance);
                else
                    mutatedSolution = mutateGene(element);
                nextGeneration.push_back(mutatedSolution);
            }

            value = distribution(gen);
            if(value <= crossover_factor){
                Solution child;
                if (distribution(gen) > 0.1)
                    child = crossover(element, selectParent(population, element, tour_size));
                else
                    child = crossoverOX(element, selectParent(population, element, tour_size));

                nextGeneration.push_back(child);
            }
        }

        population = nextGeneration;
        iteration++;
    }

    if (file.is_open())
        file.close();

    return bestSolution;
}

Solution Genetic::mutate(const Solution& solution, const ProblemInstance& instance) {
    std::vector<std::pair<int,int>> positions;
    for (size_t r = 0; r < solution.routes.size(); ++r)
        for (size_t i = 0; i < solution.routes[r].size(); ++i)
            positions.emplace_back(r, i);

    if (positions.size() < 2) return solution;

    std::uniform_int_distribution<> dist(0, positions.size() - 1);
    auto [r1, i1] = positions[dist(gen)];
    auto [r2, i2] = positions[dist(gen)];

    auto mutatedSolution = solution;

    std::swap(mutatedSolution.routes[r1][i1], mutatedSolution.routes[r2][i2]);

    mutatedSolution.calculateAndSetCost(instance);
    if (mutatedSolution.routeExceedingCapacity(instance) != -1)
        return solution;
    return mutatedSolution;
}

Solution Genetic::mutateGene(const Solution& solution) {
    std::vector<std::pair<int,int>> positions;
    for (size_t r = 0; r < solution.routes.size(); ++r)
        for (size_t i = 0; i < solution.routes[r].size(); ++i)
            positions.emplace_back(r, i);

    if (positions.size() < 2) return solution;

    std::uniform_int_distribution<> dist(0, positions.size() - 1);

    Solution mutated = solution;

    int numMutations = std::max(1, int(mutation_factor * positions.size()));
    for (int k = 0; k < numMutations; ++k) {
        auto [r1, i1] = positions[dist(gen)];
        auto [r2, i2] = positions[dist(gen)];

        while (r1 == r2 && i1 == i2)
            std::tie(r2, i2) = positions[dist(gen)];

        std::swap(mutated.routes[r1][i1], mutated.routes[r2][i2]);
        if (mutated.routeExceedingCapacity(instance) != -1)
            std::swap(mutated.routes[r1][i1], mutated.routes[r2][i2]);
    }

    mutated.calculateAndSetCost(instance);
    if (mutated.routeExceedingCapacity(instance) != -1)
        return solution;

    return mutated;
}

Solution Genetic::crossover(const Solution& parentA, const Solution& parentB) {
    Solution child;
    std::unordered_set<int> used;

    for (const auto& route : parentA.routes) {
        if (std::uniform_real_distribution<>(0.0, 1.0)(gen) < 0.5) {
            child.routes.push_back(route);
            used.insert(route.begin(), route.end());
        }
    }

    std::vector<int> remaining;
    for (const auto& route : parentB.routes)
        for (int id : route)
            if (!used.count(id))
                remaining.push_back(id);

    std::vector<int> current;
    int load = 0;
    for (int id : remaining) {
        int demand = instance.nodes[id - 1].demand;
        if (load + demand > instance.capacity) {
            if (!current.empty()) {
                child.routes.push_back(current);
                current.clear();
                load = 0;
            }
        }
        current.push_back(id);
        load += demand;
    }

    if (!current.empty())
        child.routes.push_back(current);

    child.calculateAndSetCost(instance);
    if (child.routeExceedingCapacity(instance) != -1)
        return parentA;
    return child;
}

Solution Genetic::crossoverOX(const Solution& parentA, const Solution& parentB){
    Solution child;
    std::unordered_set<int> used;

    std::vector<int> pathA;
    std::vector<int> pathB;

    for (const auto& route : parentA.routes) {
        pathA.insert(pathA.end(), route.begin(), route.end());
    }

    for (const auto& route : parentB.routes) {
        pathB.insert(pathB.end(), route.begin(), route.end());
    }

    std::uniform_int_distribution<> randomCustomer(0, static_cast<int>(pathA.size() / 2 - 1));
    int x1 = randomCustomer(gen);
    int x2 = std::uniform_int_distribution<>(x1 + 1, x1 + static_cast<int>(pathA.size() / 4))(gen);

    std::vector<int> newPath;
    newPath.insert(newPath.end(), pathA.begin(), pathA.begin() + x1);
    used.insert(pathA.begin(), pathA.begin() + x1);

    for (int i = x1; i < x2; ++i) {
        int customer = pathB[i];
        if (!used.count(customer)) {
            newPath.push_back(customer);
            used.insert(customer);
        }
    }

    for (size_t i = x2; i < pathA.size(); ++i) {
        int customer = pathA[i];
        if (!used.count(customer)) {
            newPath.push_back(customer);
            used.insert(customer);
        }
    }

    for (int customer : pathB) {
        if (!used.count(customer))
            newPath.push_back(customer);
    }


    std::vector<int> currentRoute;
    int currentLoad = 0;

    for (int customer : newPath) {
        if (currentLoad + instance.nodes[customer - 1].demand > instance.capacity) {
            if (!currentRoute.empty()) {
                child.routes.push_back(currentRoute);
                currentRoute.clear();
            }
            currentLoad = 0;
        }
        currentRoute.push_back(customer);
        currentLoad += instance.nodes[customer - 1].demand;
    }
    if (!currentRoute.empty())
        child.routes.push_back(currentRoute);

    child.calculateAndSetCost(instance);
    if (child.routeExceedingCapacity(instance) != -1 || !child.isValid(instance))
        return parentA;

    return child;
}

void Genetic::sortAndCalculateFitness(std::vector<Solution>& population) {
    std::sort(population.begin(), population.end(), utils::compareByCost);
    double minCost = population.front().cost;
    double maxCost = population.back().cost;
    double costRange = maxCost - minCost;

    if (costRange < 0.01 * minCost && mutation_factor < 0.3)
        mutation_factor *= 1.5;

    for (auto &solution : population) {
        if (costRange > 0) {
            double score = (solution.cost - minCost) / costRange;
            solution.fitness = 1 - score;
        } else {
            solution.fitness = 1;
        }
    }
}

bool Genetic::containsSolution(const std::vector<Solution> &solutions, const Solution &solution) {
    return std::any_of(solutions.begin(), solutions.end(),
                       [&](const Solution &s) { return s == solution; });
}

Solution Genetic::selectParent(const std::vector<Solution>& population, const Solution& otherParent, const int tourSize) {
    if (tourSize < 2)
        return selectParent(population, otherParent);

    if(population.empty())
        return otherParent;
    std::uniform_int_distribution<> distribution(0, static_cast<int>(population.size()) - 1);

    int parentId = INT_MAX;

    for(int i = 0; i < tourSize; i++) {
        auto val = distribution(gen);
        if (val < parentId)
            parentId = val;
    }

    if (population[parentId] != otherParent)
        return population[parentId];

    return population.front();
}

Solution Genetic::selectParent(const std::vector<Solution>& population, const Solution& otherParent) {
    if (population.empty())
        return otherParent;

    double totalFitness = 0.0;
    for (const auto &node: population) {
        totalFitness += node.fitness;
    }

    std::uniform_real_distribution<> distribution(0.0, totalFitness);
    double randomValue = distribution(gen);
    double cumulativeFitness = 0.0;

    for (const auto &node: population) {
        cumulativeFitness += node.fitness;
        double selectionThreshold = std::uniform_real_distribution<>(0.001, 0.4)(gen);
        if (cumulativeFitness >= randomValue * selectionThreshold && node != otherParent) {
            return node;
        }
    }

    return population.front();
}



Genetic::Genetic(ProblemInstance instance, double crossover_factor, double mutation_factor, int tour_size)
    : instance(std::move(instance))
    , crossover_factor{crossover_factor}
    , mutation_factor{mutation_factor}
    , tour_size{tour_size}
{}
