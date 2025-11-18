
using namespace std;

#ifndef FACEFINDER_H
#define FACEFINDER_H
#include "Point.h"
#include <vector>

/**
 * Adapted from https://cp-algorithms.com/geometry/planar.html
 * 
 */
namespace FaceFinder {
	/**
	 * Input: vertices = list of points with cartesian coords, adj = adjacency list of points.
	 * Returns a vector of vectors, where each vector contains a list of indices corresponding to each point.
	 * The indexing is the same as the input indexing of points (good for constructing graphs later)
	 */
  	std::vector<std::vector<size_t>> find_faces(std::vector<Point> vertices, std::vector<std::vector<size_t>> adj) {
    size_t n = vertices.size();
    std::vector<std::vector<char>> used(n);
    for (size_t i = 0; i < n; i++) {
        used[i].resize(adj[i].size());
        used[i].assign(adj[i].size(), 0);
        auto compare = [&](size_t l, size_t r) {
            Point pl = vertices[l] - vertices[i];
            Point pr = vertices[r] - vertices[i];
            if (pl.half() != pr.half())
                return pl.half() < pr.half();
            return pl.cross(pr) > 0;
        };
        std::sort(adj[i].begin(), adj[i].end(), compare);
    }
    std::vector<std::vector<size_t>> faces;
    for (size_t i = 0; i < n; i++) {
        for (size_t edge_id = 0; edge_id < adj[i].size(); edge_id++) {
            if (used[i][edge_id]) {
                continue;
            }
            std::vector<size_t> face;
            size_t v = i;
            size_t e = edge_id;
            while (!used[v][e]) {
                used[v][e] = true;
                face.push_back(v);
                size_t u = adj[v][e];
                size_t e1 = std::lower_bound(adj[u].begin(), adj[u].end(), v, [&](size_t l, size_t r) {
                    Point pl = vertices[l] - vertices[u];
                    Point pr = vertices[r] - vertices[u];
                    if (pl.half() != pr.half())
                        return pl.half() < pr.half();
                    return pl.cross(pr) > 0;
                }) - adj[u].begin() + 1;
                if (e1 == adj[u].size()) {
                    e1 = 0;
                }
                v = u;
                e = e1;
            }
            std::reverse(face.begin(), face.end());
            Point p1 = vertices[face[0]];
            double sum = 0;
            for (int j = 0; j < face.size(); ++j) {
                Point p2 = vertices[face[j]];
                Point p3 = vertices[face[(j + 1) % face.size()]];
                sum += (p2 - p1).cross(p3 - p2);
            }
            if (sum <= 0) {
                faces.insert(faces.begin(), face);
            } else {
                faces.emplace_back(face);
            }
        }
    }
    return faces;
}
/**
We know the output of facefinding algos will deliver points in order, so we just need to find consecutive dist. */
double computePointBoundaryWeight(std::vector<Point> facePointList) {
     double hullWeight = 0;
    size_t n = facePointList.size();

    for (size_t i = 0; i < n; i++) {
        auto &a = facePointList.at(i);
        auto &b = facePointList.at((i + 1) % n);  // wrap around

        hullWeight += std::sqrt(Point::distSq(a, b));
    }
    return hullWeight;

}
}
#endif