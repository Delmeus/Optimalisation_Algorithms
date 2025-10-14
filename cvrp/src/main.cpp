#include <iostream>
#include "utils/ProblemInstance.hpp"
#include "algorithms/Greedy.hpp"
#include "algorithms/Random.hpp"
#include "algorithms/Genetic.hpp"
#include "utils/Timer.hpp"

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

int main() {
    ProblemInstance instance(R"(G:\Projekty_Studia\_magisterka\algorytmy_optymalizacji\cvrp\input\A-n32-k5.vrp)");

    auto sol = Greedy::greedySolution(instance);
    std::cout << "GREEDY" << std::endl;
    std::cout  << "Is solution valid? " << std::boolalpha << sol.isValid(instance) << std::endl;
    std::cout << sol;
    sol.printCapacityPerRoute(instance);

    sol = testRandom(instance, 15);
    std::cout << "RANDOM" << std::endl;
    std::cout  << "Is solution valid? " << std::boolalpha << sol.isValid(instance) << std::endl;
    std::cout << sol;
    sol.printCapacityPerRoute(instance);

    sol = testGenetic(instance, 15);
    std::cout << "GENETIC" << std::endl;
    std::cout  << "Is solution valid? " << std::boolalpha << sol.isValid(instance) << std::endl;
    std::cout << sol;
    sol.printCapacityPerRoute(instance);
    return 0;
}
