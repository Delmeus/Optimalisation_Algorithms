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
        int dx = a.x - b.x;
        int dy = a.y - b.y;
        return static_cast<int>(std::sqrt(dx * dx + dy * dy));
    }
}
