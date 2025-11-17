//
// Created by Caelum Ellis on 17/11/2025.
//
#include "FlipCriteria.h"
#include <cmath>


// Signed cross product: orientation test
// Result of c1 * c2
// > 0 = left turn
// < 0 = right turn
// = 0 = collinear
// we set the safe rule to be >= 0, ensuring no right turns

static double cross(const std::pair<double,double>& p,
                    const std::pair<double,double>& a,
                    const std::pair<double,double>& b)
{
    return (a.first - p.first) * (b.second - p.second) -
           (a.second - p.second) * (b.first - p.first);
}

FlipResult FlipCriteria::isFlipLegal(const GraphState& gs, int u, int v)
{
    FlipResult res;

    EdgeKey edge(u, v);

    //////////////////////////////////////////////////////
    //   Must belong to exactly two adjacent triangles
    //////////////////////////////////////////////////////
    auto it = gs.edgeToTriangles.find(edge);
    if (it == gs.edgeToTriangles.end() || it->second.size() != 2)
        return res; // Hull edge → flipping would break triangulation

    const auto& t0 = gs.triangles[it->second[0]];
    const auto& t1 = gs.triangles[it->second[1]];

    // Identify the two opposing quad vertices
    auto pickOpposite = [&](const Triangle& t) {
        if (t.a != u && t.a != v) return t.a;
        if (t.b != u && t.b != v) return t.b;
        return t.c;
    };

    int a = pickOpposite(t0);
    int c = pickOpposite(t1);

    //////////////////////////////////////////////////////
    //  Convexity check
    //////////////////////////////////////////////////////
    const auto& pU = gs.points[u];
    const auto& pV = gs.points[v];
    const auto& pA = gs.points[a];
    const auto& pC = gs.points[c];

    // Intersection of diagonals test ensures convexity
    double c1 = cross(pA, pU, pC);
    double c2 = cross(pA, pV, pC);
    // Reject if concave or numerically ambiguous, we do not want false positives
    // (we'd rather reject a valid flip)
    if (c1 * c2 >= 0)
        return res;

    //////////////////////////////////////////////////////
    //  Safety check: ensure new diagonal doesn't already exist
    //  (Just to be sure, may remove later)
    //////////////////////////////////////////////////////
    if (gs.adjacency.at(a).count(c))
        return res;

    res.legal = true;
    res.a = a;
    res.b = u;
    res.c = c;
    res.d = v;

    return res;
}
