//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_RANDOM_H
#define ALGORYTMY_OPTYMALIZACJI_RANDOM_H


#include "../utils/Solution.hpp"
#include "../utils/ProblemInstance.hpp"

class Random {
public:
    static Solution randomSolution(const ProblemInstance& instance);
};


#endif //ALGORYTMY_OPTYMALIZACJI_RANDOM_H
