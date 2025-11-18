
#include "geometry.hpp"

double edgeLength(const Point &A, const Point &B) {
    double dx = A.x - B.x;
    double dy = A.y - B.y;
    return std::sqrt(dx*dx + dy*dy);
}


double cross(const Point &A, const Point &B, const Point &C) {
    return (B.x - A.x) * (C.y - A.y) - 
           (B.y - A.y) * (C.x - A.x);
}

bool pointInTriangle(const Point &p, const Point &A, const Point &B, const Point &C) {
    double c1 = cross(p, A, B);
    double c2 = cross(p, B, C);
    double c3 = cross(p, C, A);

    bool hasNeg = (c1 < 0) || (c2 < 0) || (c3 < 0);
    bool hasPos = (c1 > 0) || (c2 > 0) || (c3 > 0);

    // inside means all same sign or zero
    return !(hasNeg && hasPos);
}

Point polygonCentroid(const std::vector<Point> &poly) {
    double A = 0, Cx = 0, Cy = 0;
    int n = poly.size();

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        double cross = poly[i].x * poly[j].y - poly[j].x * poly[i].y;
        A += cross;
        Cx += (poly[i].x + poly[j].x) * cross;
        Cy += (poly[i].y + poly[j].y) * cross;
    }

    A *= 0.5;
    Cx /= (6 * A);
    Cy /= (6 * A);

    return {Cx, Cy};
}
