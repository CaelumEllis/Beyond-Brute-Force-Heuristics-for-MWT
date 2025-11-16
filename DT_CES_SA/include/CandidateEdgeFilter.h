//
// Created by Caelum Ellis on 16/11/2025.
//

#ifndef CANDIDATEEDGEFILTER_H
#define CANDIDATEEDGEFILTER_H

#include <vector>
#include "Coordinate.h"

class CandidateEdgeFilter {
public:
    static std::vector<std::pair<int,int>> filter(
        const std::vector<Coordinate>& points,
        const std::vector<std::pair<int,int>>& dtEdges
    );
};

#endif //CANDIDATEEDGEFILTER_H
