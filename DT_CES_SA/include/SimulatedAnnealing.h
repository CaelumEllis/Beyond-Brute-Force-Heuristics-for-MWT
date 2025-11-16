//
// Created by Caelum Ellis on 16/11/2025.
//

#ifndef SIMULATEDANNEALING_H
#define SIMULATEDANNEALING_H

#include <vector>
#include "Coordinate.h"

class SimulatedAnnealing {
public:
    std::vector<std::pair<int,int>> optimize(
        const std::vector<Coordinate>& points,
        const std::vector<std::pair<int,int>>& candidateEdges
    );
};


#endif //SIMULATEDANNEALING_H
