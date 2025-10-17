//
// Created by Antek on 14.10.2025.
//

#pragma once

#include <cmath>
#include "Solution.hpp"

namespace utils {

    struct Node {
        int id;
        int x;
        int y;
        int demand;
    };

    inline int euclidean_distance(const Node& a, const Node& b) {
        double dx = static_cast<double>(a.x) - static_cast<double>(b.x);
        double dy = static_cast<double>(a.y) - static_cast<double>(b.y);
        return static_cast<int>(std::floor(std::sqrt(dx * dx + dy * dy) + 0.5));
    }

    inline bool compareByCost(const Solution &a, const Solution &b) {
        return a.cost < b.cost;
    }

}
