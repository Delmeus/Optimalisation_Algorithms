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
#include <random>
#include <unordered_set>
#include <utility>
#include <ranges>

constexpr double MINIMAL_REQUIRED_FITNESS = 0.7;
constexpr int MINIMAL_NUMBER_OF_INDIVIDUALS = 50;
constexpr double ALLOW_INTO_NEXT_GENERATION_THRESHOLD = 0.7;

Solution Genetic::solve(size_t max_population_size, int max_number_of_generations, int id, double mutation_factor, double crossover_factor) {
    std::ofstream file;
    file.open("../../output/genetic/generations/gen" + std::to_string(id) + ".csv");
    std::vector<Solution> population;
    population.push_back(Greedy::greedySolution(instance));
    while(population.size() < max_population_size){
        population.push_back(Random::randomSolution(instance));
    }
    Solution bestSolution;
    bestSolution.cost = INT_MAX;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    Timer timer;
    timer.start();
    int iteration = 0;
    while(iteration < max_number_of_generations){
        calculateFitness(population);
        if(bestSolution.cost > population[0].cost) {
            bestSolution = population[0];
            timer.stop();
            bestSolution.timeFound = timer.mili();
        }
        std::ranges::sort(population | std::views::transform(&Solution::fitness));
        if (file.is_open())
            utils::writeGenerationToFile(file, population);
        for(int i = static_cast<int>(population.size()); i >= 0; i--){
            if((population[i].fitness < MINIMAL_REQUIRED_FITNESS && i > MINIMAL_NUMBER_OF_INDIVIDUALS * 2) || i > max_population_size){
                population.pop_back();
            }
            else
                break;
        }

        std::vector<Solution> nextGeneration;
        for(auto& element : population){
            if((element.fitness > ALLOW_INTO_NEXT_GENERATION_THRESHOLD || static_cast<int>(nextGeneration.size()) < MINIMAL_NUMBER_OF_INDIVIDUALS) && !containsSolution(
                    nextGeneration, element)) {
                nextGeneration.push_back(element);
            }

            double value = distribution(generator);
            if(1 - value <= mutation_factor){
                Solution mutatedSolution = mutate(element, instance);
                mutatedSolution.calculateAndSetCost(instance);
                if(!containsSolution(nextGeneration, mutatedSolution)) {
                    nextGeneration.push_back(mutatedSolution);
                }
            }
            value = distribution(generator);
            if(1 - value <= crossover_factor){
                auto child = crossover(element, selectParent(population, element));

                if(!containsSolution(nextGeneration, child)) {
                    nextGeneration.push_back(child);
                }
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

    std::random_device rd;
    std::mt19937 gen(rd());
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

Solution Genetic::crossover(const Solution& parentA, const Solution& parentB) {
    Solution child;
    std::unordered_set<int> used;

    std::random_device rd;
    std::mt19937 gen(rd());

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

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> randomCustomer(0, static_cast<int>(parentA.routes.size() / 2 - 1));
    std::vector<int> pathA;
    std::vector<int> pathB;

    for (const auto& route : parentA.routes) {
        pathA.insert(pathA.end(), route.begin(), route.end());
    }

    for (const auto& route : parentB.routes) {
        pathB.insert(pathB.end(), route.begin(), route.end());
    }

    int x1 = randomCustomer(gen);
    int x2 = std::uniform_int_distribution<>(x1 + 1, x1 + static_cast<int>(parentA.routes.size() / 4))(gen);

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

void Genetic::calculateFitness(std::vector<Solution>& population) {
    std::sort(population.begin(), population.end(), utils::compareByCost);
    double minCost = population.front().cost;
    double maxCost = population.back().cost;

    double costRange = maxCost - minCost;

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

Solution Genetic::selectParent(const std::vector<Solution>& population, const Solution& otherParent) {
    if(population.empty())
        return otherParent;

    std::random_device rd;
    std::mt19937 gen(rd());

    double totalFitness = 0.0;
    for (const auto& node : population) {
        totalFitness += node.fitness;
    }

    std::uniform_real_distribution<> distribution(0.0, totalFitness);
    double randomValue = distribution(gen);
    double cumulativeFitness = 0.0;

    for (const auto& node : population) {
        cumulativeFitness += node.fitness;
        double selectionThreshold = std::uniform_real_distribution<>(0.001, 0.4)(gen);
        if (cumulativeFitness >= randomValue * selectionThreshold && node != otherParent) {
            return node;
        }
    }

    return population.front();
}

// void Genetic::writeGenerationToFile(std::ofstream& file, const std::vector<Solution>& population) {
//     int cumulativeCost = 0;
//     for (const auto& node : population) {
//         cumulativeCost += node.cost;
//     }
//     float avg = static_cast<float>(cumulativeCost) / static_cast<float>(population.size());
//     file << population.front().cost << "," << population.back().cost << "," << avg << std::endl;
// }

Genetic::Genetic(ProblemInstance instance) : instance(std::move(instance)) {}
