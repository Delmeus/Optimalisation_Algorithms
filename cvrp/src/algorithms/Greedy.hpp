//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_GREEDY_H
#define ALGORYTMY_OPTYMALIZACJI_GREEDY_H

#include "../utils/Solution.hpp"
#include "../utils/ProblemInstance.hpp"

class Greedy {
public:
    static Solution greedySolution(ProblemInstance& instance, int firstCustomer = -1);
};


#endif //ALGORYTMY_OPTYMALIZACJI_GREEDY_H
