//
// Created by Antek on 17.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_TABUSEARCH_H
#define ALGORYTMY_OPTYMALIZACJI_TABUSEARCH_H

#include "../utils/Solution.hpp"
#include "../utils/ProblemInstance.hpp"

struct Move {
    std::pair<int, int> customers;
//    std::pair<int, int> moveCustomer;
//    int tabuTime = 100;
};

struct Neighbour {
    Solution solution;
    Move move;
};

class TabuSearch {
public:
    Solution solve(int max_number_of_iterations, int id);

    explicit TabuSearch(ProblemInstance instance);

private:
    ProblemInstance instance;
    std::vector<std::pair<Move, int>> tabuList;

    Neighbour findBestNeighbour(const std::vector<Neighbour>& v);
    std::vector<Neighbour> generateNeighbours(const Solution& solution);

    bool isMoveTabu(const Move& move);
};


#endif //ALGORYTMY_OPTYMALIZACJI_TABUSEARCH_H
