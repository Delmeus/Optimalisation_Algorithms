#include <iostream>
#include <thread>
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
        iteration++;
    }
    return bestSolution;
}

Solution testGenetic(const ProblemInstance& instance, int iterationLimit, int id, int maxPopulationSize = 500) {
    Genetic genetic(instance);
    return genetic.solve(maxPopulationSize, iterationLimit, id);
}

Solution testTabu(const ProblemInstance& instance, int iterationLimit, int id) {
    TabuSearch ts(instance);
    return ts.solve(iterationLimit, id);
}

void runRandomTests(const ProblemInstance& instance, int numberOfTests, int iterationLimit) {
    int bestSolutionCost = INT_MAX;
    Solution sol;

    for (int i = 0; i < numberOfTests; i++) {
        std::cout << "[RANDOM] iteration " << i + 1 << "/" << numberOfTests << std::endl;
        sol = testRandom(instance, iterationLimit);

        if (sol.cost < bestSolutionCost) {
            sol.saveFullSolutionToFile(instance, "random/" + instance.name + "_random_best_solution.csv");
            sol.saveFullSolutionToFileSolFormat(instance, "random/" + instance.name + "_random_best_solution.sol");
            bestSolutionCost = sol.cost;
        }
        sol.saveSolutionToFile("random/" + instance.name + "_random_costs.csv");
    }

    std::cout << "[RANDOM] finished. Best cost = " << bestSolutionCost << std::endl;
}

void runGeneticTests(const ProblemInstance& instance, int testNumber, int iterationLimit) {
    Solution sol;

    std::cout << "[GENETIC] " << testNumber << " running" << std::endl;
    sol = testGenetic(instance, iterationLimit, testNumber);

    std::lock_guard<std::mutex> lock(fileGeneticMutex);
    sol.saveFullSolutionToFile(instance, "genetic/" + instance.name + "_genetic_best_solution.csv");
    sol.saveFullSolutionToFileSolFormat(instance, "genetic/" + instance.name + "_genetic_best_solution.sol");
    sol.saveSolutionToFile("genetic/" + instance.name + "_genetic_costs.csv");

    std::cout << "[GENETIC] finished. Best cost = " << sol.cost << std::endl;
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
        ProblemInstance instance("../../input/A-n32-k5.vrp");
        // std::cout << testGenetic(instance, 10000, 0);
        std::cout << testTabu(instance, 10000, 0);
        // std::cout << Greedy::greedySolution(instance);
    }
    else {
        std::string instanceName = argv[1];
        int iterationLimit = std::stoi(argv[2]);

        ProblemInstance instance("../../input/" + instanceName);

        Timer timer;
        timer.start();
        auto sol = Greedy::greedySolution(instance);
        timer.stop();
        sol.timeFound = timer.mili();
        sol.saveFullSolutionToFile(instance, "greedy/" + instance.name + "_greedy.csv");

        std::thread randomThread(runRandomTests, std::cref(instance), NUMBER_OF_TESTS, iterationLimit);

        std::vector<std::thread> geneticThreads;
        std::vector<std::thread> tabuThreads;

        for (int i = 0; i < NUMBER_OF_TESTS; i++) {
            geneticThreads.emplace_back(runGeneticTests, std::cref(instance), i, iterationLimit);
        }

        for (int i = 0; i < NUMBER_OF_TESTS; i++) {
            tabuThreads.emplace_back(runTabuTests, std::cref(instance), i, iterationLimit);
        }

        randomThread.join();

        for (auto& t : geneticThreads)
            t.join();

        for (auto& t : tabuThreads)
            t.join();
    }

    return 0;
}
