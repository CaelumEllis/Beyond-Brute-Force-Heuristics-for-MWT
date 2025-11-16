//
// Created by Caelum Ellis on 17/11/2025.
//

#ifndef GRAPHSTATE_H
#define GRAPHSTATE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include "DelaunayWrapper.h"

struct EdgeKey {
    int u, v;
    EdgeKey(int a, int b) {
        if (a < b) { u = a; v = b; }
        else { u = b; v = a; }
    }
    bool operator==(const EdgeKey &o) const { return u == o.u && v == o.v; }
};

struct EdgeKeyHash {
    size_t operator()(const EdgeKey& e) const {
        return std::hash<int>()(e.u) ^ (std::hash<int>()(e.v) << 1);
    }
};

class GraphState {

public:
    std::vector<std::pair<double,double>> points;
    std::vector<Edge> edges;
    std::vector<Triangle> triangles;

    // adjacency list (for quick graph traversal if needed)
    std::unordered_map<int, std::unordered_set<int>> adjacency;

    // edge -> list of triangle indices that reference it
    std::unordered_map<EdgeKey, std::vector<int>, EdgeKeyHash> edgeToTriangles;

    GraphState() = default;

    GraphState(const DTResult &dt, const std::vector<std::pair<double,double>>& pts)
        : points(pts), edges(dt.edges), triangles(dt.triangles)
    {
        buildAdjacency();
        buildEdgeTriangleMap();
    }

private:

    void buildAdjacency() {
        for (auto &e : edges) {
            adjacency[e.u].insert(e.v);
            adjacency[e.v].insert(e.u);
        }
    }

    void buildEdgeTriangleMap() {
        for (size_t i = 0; i < triangles.size(); ++i) {
            auto &t = triangles[i];
            addTriangleEdge(t.a, t.b, i);
            addTriangleEdge(t.b, t.c, i);
            addTriangleEdge(t.c, t.a, i);
        }
    }

    void addTriangleEdge(int x, int y, int triIdx) {
        EdgeKey key(x, y);
        edgeToTriangles[key].push_back(triIdx);
    }
};

#endif //GRAPHSTATE_H
