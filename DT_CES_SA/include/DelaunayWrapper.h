//
// Created by Caelum Ellis on 17/11/2025.
//

#ifndef DELAUNAYWRAPPER_H
#define DELAUNAYWRAPPER_H

// Uses SimpleDelaunay (MIT License) - https://github.com/jeanbega/SimpleDelaunay
#include "SimpleDelaunay.h"
#include <vector>
#include <utility>
#include <cmath>

struct Edge {
    int u;
    int v;
    double weight;
};

struct Triangle {
    int a, b, c;
};

struct DTResult {
    std::vector<Edge> edges;          // unique weighted edge set
    std::vector<Triangle> triangles;  // exact triangulation connectivity
};

class DelaunayWrapper {

public:
    // Takes points of type (x,y) and returns
    // DTResult (2D delaunay) of type {edges, triangles}
    static DTResult translateOutput(const std::vector<std::pair<double,double>>& points);
};

#endif //DELAUNAYWRAPPER_H
