
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <cmath>

class Point {
        public:
    double x, y;


    Point(double x_, double y_) : x{x_}, y{y_} {};
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
