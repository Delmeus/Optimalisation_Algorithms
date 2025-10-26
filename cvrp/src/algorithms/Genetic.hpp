//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_GENETICALGORITHM_H
#define ALGORYTMY_OPTYMALIZACJI_GENETICALGORITHM_H


#include "../utils/Solution.hpp"
#include "../utils/ProblemInstance.hpp"

class Genetic {
public:
    Solution solve(size_t max_population_size, int max_number_of_generations, int id, double crossover_factor = 0.7);

    explicit Genetic(ProblemInstance instance, double mutation_factor = 0.1);

private:
    ProblemInstance instance;
    double mutation_factor;

    static Solution mutate(const Solution& solution, const ProblemInstance& instance);
    Solution mutateGene(const Solution& solution);
    Solution crossover(const Solution& parentA, const Solution& parentB);
    Solution crossoverOX(const Solution& parentA, const Solution& parentB);
    static void calculateFitness(std::vector<Solution>& population);
    static bool containsSolution(const std::vector<Solution> &solutions, const Solution &solution);
    static Solution selectParent(const std::vector<Solution>& population, const Solution& otherParent);
    // static void writeGenerationToFile(std::ofstream& file, const std::vector<Solution>& population);
};


#endif //ALGORYTMY_OPTYMALIZACJI_GENETICALGORITHM_H
