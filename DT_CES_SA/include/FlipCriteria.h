//
// Created by Caelum Ellis on 17/11/2025.
//

#ifndef FLIPCRITERIA_H
#define FLIPCRITERIA_H

#include "GraphState.h"
#include "DelaunayWrapper.h"
class GraphState;
    struct FlipResult {
        bool legal = false;
        int a,b,c,d;
    };

class FlipCriteria {
public:
    // Determines whether flipping edge (u,v) results in
    // a valid planar triangulation.
     // Constraints for legality:
     //     - Edge must belong to exactly 2 triangles (interior edge).
     //     - The two triangles must form a convex quadrilateral.
     //     - (Practical safeguard) The alternate diagonal must not already exist.
    static FlipResult isFlipLegal(const GraphState& gs, int u, int v);
};

#endif //FLIPCRITERIA_H
