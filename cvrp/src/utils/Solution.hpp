//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_SOLUTION_H
#define ALGORYTMY_OPTYMALIZACJI_SOLUTION_H

#include <set>
#include <vector>
#include <ostream>

class ProblemInstance;

class Solution {
public:
    std::vector<std::vector<int>> routes;
    int cost = 0;
    double fitness;
    bool isValid(const ProblemInstance& instance);
    void calculateAndSetCost(const ProblemInstance& instance);
    void printCapacityPerRoute(const ProblemInstance& instance);

    int routeExceedingCapacity(const ProblemInstance& instance);

    bool operator==(const Solution &other) const {
        return cost == other.cost && routes == other.routes;
    }

    friend std::ostream& operator<<(std::ostream& os, const Solution& solution) {
        for(int i = 1; i < solution.routes.size() + 1; i++) {
            os << "Route #" << i << ": ";
            for (int stop : solution.routes[i - 1]) {
                os << stop << " ";
            }
            os << std::endl;
        }
        os << "Cost " << solution.cost << std::endl;
        return os;
    }
};


#endif //ALGORYTMY_OPTYMALIZACJI_SOLUTION_H
