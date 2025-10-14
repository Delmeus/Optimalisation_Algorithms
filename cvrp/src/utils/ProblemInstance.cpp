//
// Created by Antek on 12.10.2025.
//

#include "ProblemInstance.hpp"

void ProblemInstance::trim(std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        s.clear();
        return;
    }
    s = s.substr(start, end - start + 1);
}

void ProblemInstance::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::string line;
    enum Section { HEADER, NODE_COORD, DEMAND, DEPOT } section = HEADER;

    std::vector<std::pair<int, std::pair<int, int>>> coords;
    std::vector<std::pair<int, int>> demands;

    while (std::getline(file, line)) {
        trim(line);
        if (line.empty())
            continue;
        if (line == "EOF")
            break;
        if (line == "NODE_COORD_SECTION") {
            section = NODE_COORD;
            continue;
        }
        else if (line == "DEMAND_SECTION") {
            section = DEMAND;
            continue;
        }
        else if (line == "DEPOT_SECTION") {
            section = DEPOT;
            continue;
        }

        if (section == HEADER) {
            if (line.find(':') != std::string::npos) {
                std::string key = line.substr(0, line.find(':'));
                std::string value = line.substr(line.find(':') + 1);
                trim(key);
                trim(value);

                if (key == "NAME")
                    name = value;
                else if (key == "COMMENT")
                    comment = value;
                else if (key == "TYPE")
                    type = value;
                else if (key == "DIMENSION")
                    dimension = std::stoi(value);
                else if (key == "CAPACITY")
                    capacity = std::stoi(value);
                else if (key == "EDGE_WEIGHT_TYPE")
                    edge_weight_type = value;
            }
        }
        else if (section == NODE_COORD) {
            std::istringstream iss(line);
            int id, x, y;
            if (!(iss >> id >> x >> y)) continue;
            coords.push_back({id, {x, y}});
        }
        else if (section == DEMAND) {
            std::istringstream iss(line);
            int id, d;
            if (!(iss >> id >> d)) continue;
            demands.push_back({id, d});
        }
        else if (section == DEPOT) {
            std::istringstream iss(line);
            int id;
            if (!(iss >> id)) continue;
            if (id == -1) continue;
            depots.push_back(id);
        }
    }

    nodes.resize(dimension);
    for (int i = 0; i < dimension; ++i) {
        nodes[i].id = i + 1;
        nodes[i].x = 0;
        nodes[i].y = 0;
        nodes[i].demand = 0;
    }

    for (auto &c : coords) {
        int idx = c.first - 1;
        if (idx >= 0 && idx < dimension) {
            nodes[idx].x = c.second.first;
            nodes[idx].y = c.second.second;
        }
    }
    for (auto &d : demands) {
        int idx = d.first - 1;
        if (idx >= 0 && idx < dimension) {
            nodes[idx].demand = d.second;
        }
    }

    file.close();
}