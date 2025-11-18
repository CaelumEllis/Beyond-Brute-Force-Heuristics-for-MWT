
#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <cmath>

struct Point {
    double x, y;
};

struct Triangle {
    int a, b, c;   // indices into point array
};

struct Edge {
    int u, v;  // always sorted: u < v
};

double edgeLength(const Point &A, const Point &B);


double cross(const Point &A, const Point &B, const Point &C);
bool pointInTriangle(const Point &p, const Point &A, const Point &B, const Point &C);
Point polygonCentroid(const std::vector<Point> &poly);

#endif
