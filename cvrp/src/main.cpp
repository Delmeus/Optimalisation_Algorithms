#include <iostream>
#include <thread>
#include "utils/ProblemInstance.hpp"
#include "algorithms/Greedy.hpp"
#include "algorithms/Random.hpp"
#include "algorithms/Genetic.hpp"
#include "utils/Timer.hpp"

constexpr int NUMBER_OF_TESTS = 10;

Solution testRandom(const ProblemInstance& instance, long long timeLimit) {
    Timer timer;
    timer.start();
    Solution bestSolution;
    bestSolution.cost = INT_MAX;
    while (timer.mili() < timeLimit * 1000) {
        auto sol = Random::randomSolution(instance);
        if (sol.cost < bestSolution.cost)
            bestSolution = sol;
        timer.stop();
    }
    return bestSolution;
}

Solution testGenetic(const ProblemInstance& instance, long long timeLimit, int maxPopulationSize = 500) {
    Genetic genetic(instance);
    return genetic.solve(maxPopulationSize, timeLimit);
}

void runRandomTests(const ProblemInstance& instance, int numberOfTests, int timeLimit) {
    int bestSolutionCost = INT_MAX;
    Solution sol;

    for (int i = 0; i < numberOfTests; i++) {
        std::cout << "[RANDOM] iteration " << i + 1 << "/" << numberOfTests << std::endl;
        sol = testRandom(instance, timeLimit);

        if (sol.cost < bestSolutionCost) {
            sol.saveFullSolutionToFile(instance, instance.name + "_random_best_solution.csv");
            bestSolutionCost = sol.cost;
        }
        sol.saveSolutionToFile(instance.name + "_random_costs.csv");
    }

    std::cout << "[RANDOM] finished. Best cost = " << bestSolutionCost << std::endl;
}

void runGeneticTests(const ProblemInstance& instance, int numberOfTests, int timeLimit) {
    int bestSolutionCost = INT_MAX;
    Solution sol;

    for (int i = 0; i < numberOfTests; i++) {
        std::cout << "[GENETIC] iteration " << i + 1 << "/" << numberOfTests << std::endl;
        sol = testGenetic(instance, timeLimit);

        if (sol.cost < bestSolutionCost) {
            sol.saveFullSolutionToFile(instance, instance.name + "_genetic_best_solution.csv");
            bestSolutionCost = sol.cost;
        }
        sol.saveSolutionToFile(instance.name + "_genetic_costs.csv");
    }

    std::cout << "[GENETIC] finished. Best cost = " << bestSolutionCost << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Program requires 2 arguments! <filename> <time_limit_in_seconds>" << std::endl;
        return -1;
    }
    std::string instanceName = argv[1];
    int timeLimit = std::stoi(argv[2]);

    ProblemInstance instance(R"(G:\Projekty_Studia\_magisterka\algorytmy_optymalizacji\cvrp\input\)" + instanceName);

    auto sol = Greedy::greedySolution(instance);
    sol.saveFullSolutionToFile(instance, instance.name + "_greedy.csv");

    std::thread randomThread(runRandomTests, std::cref(instance), NUMBER_OF_TESTS, timeLimit);
    std::thread geneticThread(runGeneticTests, std::cref(instance), NUMBER_OF_TESTS, timeLimit);

    randomThread.join();
    geneticThread.join();

    return 0;
}
