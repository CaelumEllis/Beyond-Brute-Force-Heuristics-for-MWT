//
// Created by Caelum Ellis on 17/11/2025.
//
#include "DelaunayWrapper.h"
#include <EdgeKey.h>
#include <unordered_set>
#include <set>
#include <EdgeKey.h>

DTResult DelaunayWrapper::translateOutput(const std::vector<std::pair<double, double>>& points)
{
    DTResult result;
    std::unordered_set<EdgeKey, EdgeKeyHash> uniquePairs;
    // std::set<std::pair<int,int>> uniquePairs;

    // Convert (x,y) pairs to flat format for SimpleDelaunay
    std::vector<double> flat;
    flat.reserve(points.size() * 2);

    for (auto& p : points) {
        flat.push_back(p.first);
        flat.push_back(p.second);
    }

    // Run triangulation
    std::vector<int> connectivity = SimpleDelaunay::compute<2>(flat);

    // Convert triangle triples and extract edges
    for (size_t i = 0; i < connectivity.size(); i += 3) {
        int a = connectivity[i];
        int b = connectivity[i + 1];
        int c = connectivity[i + 2];

        // Store triangle
        result.triangles.emplace_back(Triangle{a, b, c});

        auto addEdge = [&](int x, int y){
            if (x > y) std::swap(x, y);
            uniquePairs.insert({x, y});
        };

        addEdge(a,b);
        addEdge(b,c);
        addEdge(c,a);
    }

    // Convert deduplicated edges to weighted edges
    for (const auto& p : uniquePairs) {

        auto& p1 = points[p.u];
        auto& p2 = points[p.v];

        double dx = p1.first - p2.first;
        double dy = p1.second - p2.second;

        result.edges.push_back(Edge{
            p.u, p.v,
            std::sqrt(dx * dx + dy * dy)
        });
    }


    return result;
}

