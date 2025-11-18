
#include "hull.hpp"
#include <algorithm>

static bool cmpXY(const Point &a, const Point &b) {
    return (a.x < b.x) || (a.x == b.x && a.y < b.y);
}

std::vector<int> convexHullIndices(const std::vector<Point> &P) {
    int n = P.size();
    std::vector<int> ids(n);
    for (int i = 0; i < n; i++) ids[i] = i;

    std::sort(ids.begin(), ids.end(), [&](int i, int j){
        return cmpXY(P[i], P[j]);
    });

    std::vector<int> hull;

    // lower hull
    for (int id : ids) {
        while (hull.size() >= 2 &&
               cross(P[hull[hull.size()-2]], P[hull.back()], P[id]) <= 0)
            hull.pop_back();
        hull.push_back(id);
    }

    // upper hull
    int lowerSize = hull.size();
    for (int i = ids.size() - 2; i >= 0; i--) {
        int id = ids[i];
        while ((int)hull.size() > lowerSize &&
               cross(P[hull[hull.size()-2]], P[hull.back()], P[id]) <= 0)
            hull.pop_back();
        hull.push_back(id);
    }

    hull.pop_back(); // last is duplicate
    return hull;
}
