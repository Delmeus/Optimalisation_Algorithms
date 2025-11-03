#include <iostream>
#include <thread>
#include <climits>
#include "utils/ProblemInstance.hpp"
#include "algorithms/Greedy.hpp"
#include "algorithms/Random.hpp"
#include "algorithms/Genetic.hpp"
#include "utils/Timer.hpp"
#include "algorithms/TabuSearch.hpp"

constexpr int NUMBER_OF_TESTS = 10;

std::mutex fileGeneticMutex;

Solution testRandom(const ProblemInstance& instance, int iterationLimit) {
    Timer timer;
    timer.start();
    Solution bestSolution;
    bestSolution.cost = INT_MAX;
    int iteration = 0;
    while (iteration < iterationLimit) {
        auto sol = Random::randomSolution(instance);
        if (sol.cost < bestSolution.cost) {
            bestSolution = sol;
            timer.stop();
            bestSolution.timeFound = timer.mili();
        }
        sol.saveSolutionToFile("random/" + instance.name + "_random_costs.csv");
        iteration++;
    }
    return bestSolution;
}

Solution testGenetic(const ProblemInstance& instance, int iterationLimit, int id, int maxPopulationSize = 100, double crossover_factor = 0.7, double mutation_factor = 0.1, int tour_size = -1) {
    Genetic genetic(instance, crossover_factor, mutation_factor, tour_size);
    return genetic.solve(maxPopulationSize, iterationLimit, id);
}

Solution testTabu(const ProblemInstance& instance, int iterationLimit, int id) {
    TabuSearch ts(instance);
    return ts.solve(iterationLimit, id);
}

void runRandomTests(const ProblemInstance& instance, int iterationLimit) {
    Solution sol;
    sol = testRandom(instance, iterationLimit);
    sol.saveFullSolutionToFile(instance, "random/" + instance.name + "_random_best_solution.csv");
    sol.saveFullSolutionToFileSolFormat(instance, "random/" + instance.name + "_random_best_solution.sol");
    std::cout << "[RANDOM] finished. Best cost = " << sol.cost << std::endl;
}

void runGeneticTests(const ProblemInstance& instance, int testNumber, int iterationLimit, int maxPopulationSize = 100, double crossover_factor = 0.7, double mutation_factor = 0.1, int tour_size = -1) {
    Solution sol;

    std::cout << "[GENETIC] " << testNumber << " running" << std::endl;
    sol = testGenetic(instance, iterationLimit, testNumber, maxPopulationSize, crossover_factor, mutation_factor, tour_size);

    std::lock_guard<std::mutex> lock(fileGeneticMutex);
    sol.saveFullSolutionToFile(instance, "genetic/" + instance.name + "_genetic_best_solution.csv");
    sol.saveFullSolutionToFileSolFormat(instance, "genetic/" + instance.name + "_genetic_best_solution.sol");
    sol.saveSolutionToFile("genetic/" + instance.name + "_genetic_costs.csv");

    std::cout << "[GENETIC] " << testNumber << " finished. Best cost = " << sol.cost << std::endl;
}

void runTabuTests(const ProblemInstance& instance, int testNumber, int iterationLimit) {
    Solution sol;

    std::cout << "[TABU SEARCH] " << testNumber << " running" << std::endl;
    sol = testTabu(instance, iterationLimit, testNumber);

    std::lock_guard<std::mutex> lock(fileGeneticMutex);
    sol.saveFullSolutionToFile(instance, "tabu/" + instance.name + "_tabu_best_solution.csv");
    sol.saveFullSolutionToFileSolFormat(instance, "tabu/" + instance.name + "_tabu_best_solution.sol");
    sol.saveSolutionToFile("tabu/" + instance.name + "_tabu_costs.csv");

    std::cout << "[TABU SEARCH] finished. Best cost = " << sol.cost << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        ProblemInstance instance("../../input/A-n60-k9.vrp");
//        std::cout << testTabu(instance, 10000, 0);
        for (int i = 2; i < instance.dimension + 1; i++) {
            auto sol = Greedy::greedySolution(instance, i);
            sol.calculateAndSetCost(instance);
            sol.saveFullSolutionToFile(instance, "greedy/" + instance.name + "_greedy.csv");
            sol.saveSolutionToFile("greedy/" + instance.name + "_greedy_costs.csv");
        }
    }
    else {
        std::string instanceName = argv[1];
        int iterationLimit = std::stoi(argv[2]);

        ProblemInstance instance("../../input/" + instanceName);
        for (int i = 2; i < instance.dimension + 1; i++) {
            auto sol = Greedy::greedySolution(instance, i);
            sol.saveFullSolutionToFile(instance, "greedy/" + instance.name + "_greedy.csv");
            sol.saveSolutionToFile("greedy/" + instance.name + "_greedy_costs.csv");
        }
        std::thread randomThread(runRandomTests, std::cref(instance), iterationLimit);

        std::vector<std::thread> geneticThreads;
        std::vector<std::thread> tabuThreads;

        for (int i = 0; i < NUMBER_OF_TESTS; i++) {
            geneticThreads.emplace_back(runGeneticTests, std::cref(instance), i, iterationLimit, 500, 0.7, 0.1, -1);
        }

        for (int i = 0; i < NUMBER_OF_TESTS; i++) {
            tabuThreads.emplace_back(runTabuTests, std::cref(instance), i, iterationLimit);
        }

        for (auto &t: geneticThreads)
            t.join();

        for (auto &t: tabuThreads)
            t.join();
        randomThread.join();
    }

    return 0;
}
