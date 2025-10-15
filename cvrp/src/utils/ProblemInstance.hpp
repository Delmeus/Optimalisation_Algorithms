//
// Created by Antek on 12.10.2025.
//

#ifndef ALGORYTMY_OPTYMALIZACJI_GRAPH_H
#define ALGORYTMY_OPTYMALIZACJI_GRAPH_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "common_utils.hpp"

enum Section {
    HEADER,
    NODE_COORD,
    DEMAND,
    DEPOT
};

class ProblemInstance {
public:
    std::string name;
    std::string comment;
    std::string type;
    int dimension = 0;
    int capacity = 0;
    std::string edge_weight_type;

    std::vector<utils::Node> nodes;
    std::vector<int> depots;

    ProblemInstance() = default;

    explicit ProblemInstance(const std::string& filename) {
        loadFromFile(filename);
    }

    void loadFromFile(const std::string& filename);

    friend std::ostream& operator<<(std::ostream& os, const ProblemInstance& instance)
    {
        os << "DIMENSION = " << instance.dimension << std::endl;
        os << "TRUCK CAPACITY = " << instance.capacity << std::endl;
        os << "NODES: " << std::endl;
        for(auto node : instance.nodes) {
            os << node.x << " " << node.y << "\tDEMAND = " << node.demand << std::endl;
        }
        os << "DEPOTS: " << std::endl;
        for(int depot : instance.depots) {
            os << depot << std::endl;
        }
        return os;
    }

private:
    void trim(std::string &s);
};



#endif //ALGORYTMY_OPTYMALIZACJI_GRAPH_H
