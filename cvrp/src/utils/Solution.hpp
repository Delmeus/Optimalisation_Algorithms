//
// Created by Antek on 14.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_SOLUTION_H
#define ALGORYTMY_OPTYMALIZACJI_SOLUTION_H

#include <set>
#include <vector>
#include <ostream>
#include <algorithm>

class ProblemInstance;

class Solution {
public:
    std::vector<std::vector<int>> routes;
    int cost = 0;
    double fitness;
    long long timeFound;

    bool isValid(const ProblemInstance& instance);
    void calculateAndSetCost(const ProblemInstance& instance);
    void printCapacityPerRoute(const ProblemInstance& instance);
    void saveFullSolutionToFile(const ProblemInstance& instance, const std::string& filename);
    void saveFullSolutionToFileSolFormat(const ProblemInstance& instance, const std::string& filename);
    void saveSolutionToFile(const std::string& filename) const;

    int routeExceedingCapacity(const ProblemInstance& instance);

    Solution& operator=(const Solution& other) {
        if (this != &other) {
            routes = other.routes;
            cost = other.cost;
            fitness = 0;
            timeFound = 0;
        }
        return *this;
    }

    bool operator==(const Solution &other) const {
        if (cost != other.cost || routes.size() != other.routes.size())
            return false;

        auto normalize = [](const std::vector<std::vector<int>>& rs) {
            std::vector<std::vector<int>> sortedRoutes = rs;
            std::sort(sortedRoutes.begin(), sortedRoutes.end());
            return sortedRoutes;
        };

        auto lhs = normalize(routes);
        auto rhs = normalize(other.routes);

        return lhs == rhs;
    }

    friend std::ostream& operator<<(std::ostream& os, const Solution& solution) {
        for(int i = 1; i < solution.routes.size() + 1; i++) {
            os << "Route #" << i << ": ";
            for (int stop : solution.routes[i - 1]) {
                os << stop - 1 << " "; // alignment to .sol format
            }
            os << std::endl;
        }
        os << "Cost " << solution.cost << std::endl;
        return os;
    }
};


#endif //ALGORYTMY_OPTYMALIZACJI_SOLUTION_H
