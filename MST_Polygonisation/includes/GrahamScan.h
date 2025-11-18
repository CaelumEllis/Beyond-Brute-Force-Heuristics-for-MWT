/**
 * geeks4geeks implementation, modified to work with curr reading of points + floating point instead of integer
 */
#ifndef GRAHAMSCAN_H
#define GRAHAMSCAN_H
#include "Point.h"
#include <vector>

namespace GrahamScan {
  // Function to find orientation of the triplet (a, b, c)
 // Returns -1 if clockwise, 1 if counter-clockwise, 0 if collinear
int orientation(Point a, Point b, Point c) {
    double v = a.x * (b.y - c.y) + 
               b.x * (c.y - a.y) + 
               c.x * (a.y - b.y);
    if (v < 0) return -1; 
    if (v > 0) return +1; 
    return 0;
}

 // Function to calculate the squared distance between two points
double distSq(Point a, Point b) {
    return (a.x - b.x) * (a.x - b.x) + 
           (a.y - b.y) * (a.y - b.y);
}

 // Function to find the convex hull of a set of 2D points
std::vector<std::pair<double, double>> findConvexHull(std::vector<std::pair<double, double>> points) {

    // Store number of points points
    int n = points.size();

    // Convex hull is not possible if there are fewer than 3 points
    // I'm just going to assume this as a precondition
    /* if (n < 3) return {{-1}}; */

    // Convert points 2D vector into vector of Point structures
    std::vector<Point> a;
    for (auto& p : points) {
        a.push_back({(double)p.first, (double)p.second});
    }

    // Find the point with the lowest y-coordinate (and leftmost in case of tie)
    Point p0 = *min_element(a.begin(), a.end(), [](Point a, Point b) {
        return std::make_pair(a.y, a.x) < std::make_pair(b.y, b.x);
    });

    // Sort points based on polar angle with respect to the reference point p0
    std::sort(a.begin(), a.end(), [&p0](const Point& a, const Point& b) {
        int o = orientation(p0, a, b);

        // If points are collinear, keep the farthest one last
        if (o == 0) {
            return distSq(p0, a) < distSq(p0, b);
        }

        // Otherwise, sort by counter-clockwise order
        return o < 0;
    });

    // Vector to store the points on the convex hull
    std::vector<Point> st;

    // Process each point to build the hull
    for (int i = 0; i < (int)a.size(); ++i) {

        // While last two points and current point make a non-left turn, remove the middle one
        while (st.size() > 1 && orientation(st[st.size() - 2], st.back(), a[i]) >= 0)
            st.pop_back();

        // Add the current point to the hull
        st.push_back(a[i]);
    }

    // If fewer than 3 points in the final hull, return {-1, -1}
    if (st.size() < 3) return std::vector<std::pair<double, double>>{{-1, -1}};

    // Convert the final hull into a vector of vectors of doubles
    std::vector<std::pair<double, double>> result;
    for (auto& p : st) {
        result.push_back(std::pair<double, double>(p.x, p.y));
    }

    return result;
}

double findConvexWeight(std::vector<std::pair<double, double>> &convexHullPoints) {
    double hullWeight = 0;
    size_t n = convexHullPoints.size();

    for (size_t i = 0; i < n; i++) {
        auto &a = convexHullPoints.at(i);
        auto &b = convexHullPoints.at((i + 1) % n);  // wrap around
        double dx = a.first - b.first;
        double dy = a.second - b.second;

        hullWeight += std::sqrt(dx*dx + dy*dy);
    }
    return hullWeight;

}
}
#endif