//
// Created by Caelum Ellis on 16/11/2025.
//
#ifndef GRAPHSTATE_H
#define GRAPHSTATE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "DelaunayWrapper.h"
#include "EdgeKey.h"

struct FlipResult;

class GraphState {

public:
    std::vector<std::pair<double,double>> points;
    std::vector<Edge> edges;
    std::unordered_map<EdgeKey, Edge, EdgeKeyHash> edgeTable;
    std::unordered_map<EdgeKey, size_t, EdgeKeyHash> edgeIndex;
    std::vector<Triangle> triangles;

    // adjacency list (for quick graph traversal if needed)
    std::unordered_map<int, std::unordered_set<int>> adjacency;

    // edge -> list of triangle indices that reference it
    std::unordered_map<EdgeKey, std::vector<int>, EdgeKeyHash> edgeToTriangles;

    GraphState() = default;

    GraphState(const DTResult &dt, const std::vector<std::pair<double,double>>& pts)
        : points(pts), edges(dt.edges), triangles(dt.triangles)
    {
        for (size_t i = 0; i < edges.size(); ++i) {
            EdgeKey k(edges[i].u, edges[i].v);
            edgeIndex[k] = i;
            edgeTable[k] = edges[i];
        }
        buildAdjacency();
        buildEdgeTriangleMap();
    }

    // Performs the flip from diagonal (b,d) to (a,c), using a validated FlipResult
    void FlipEdge(const FlipResult& f);

    bool hasEdge(int u, int v) const {
        return edgeTable.contains(EdgeKey(u,v));
    }

    const Edge* getEdge(int u, int v) const {
        auto it = edgeTable.find(EdgeKey(u,v));
        return it == edgeTable.end() ? nullptr : &(it->second);
    }

private:
    void buildAdjacency();
    void buildEdgeTriangleMap();
    void addTriangleEdge(int x, int y, int triIdx);

    void removeEdge(int u, int v);
    void addEdge(int u, int v);
};

#endif //GRAPHSTATE_H
