
using namespace std;

#ifndef FACEFINDER_H
#define FACEFINDER_H
#include "Point.h"
#include <vector>
// Replace your sort + lower_bound block with this version.

#include <algorithm>
#include <cmath>
#include <iostream>


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
  	
std::vector<std::vector<size_t>> find_faces(std::vector<Point> &vertices, std::vector<std::vector<size_t>> &adj) {
    size_t n = vertices.size();

    // basic sanity checks
    for (size_t u = 0; u < n; ++u) {
        for (size_t v : adj[u]) {
            if (v >= n) {
                std::cerr << "ERROR: adjacency contains invalid index " << v << " at vertex " << u << "\n";
            }
            if (v == u) {
                std::cerr << "WARNING: self-loop detected at vertex " << u << "\n";
            }
        }
    }

    // Verify symmetry (optional but very helpful)
    for (size_t u = 0; u < n; ++u) {
        for (size_t v : adj[u]) {
            bool found = false;
            for (size_t w : adj[v]) {
                if (w == u) { found = true; break; }
            }
            if (!found) {
                std::cerr << "WARNING: adjacency not symmetric: edge " << u << " - " << v << " missing in adj[" << v << "]\n";
            }
        }
    }

    std::vector<std::vector<char>> used(n);
    for (size_t i = 0; i < n; i++) {
        used[i].assign(adj[i].size(), 0);

        // comparator for sorting neighbors by angle around vertex i
        auto compare = [&](size_t l, size_t r) {
            Point pl = vertices[l] - vertices[i];
            Point pr = vertices[r] - vertices[i];

            int hl = pl.half();
            int hr = pr.half();
            if (hl != hr) return hl < hr;

            auto cr = pl.cross(pr);
            if (cr != 0) return cr > 0; // pl is before pr if cross > 0

            // tie-breaker: points collinear; put closer point first
            // use squared distance to avoid sqrt; assume Point::distSq exists and returns double/long double
            double dl = Point::distSq(vertices[i], vertices[l]);
            double dr = Point::distSq(vertices[i], vertices[r]);
            return dl < dr;
        };

        std::sort(adj[i].begin(), adj[i].end(), compare);
    }

    std::vector<std::vector<size_t>> faces;
    for (size_t i = 0; i < n; i++) {
        for (size_t edge_id = 0; edge_id < adj[i].size(); edge_id++) {
            if (used[i][edge_id]) continue;

            std::vector<size_t> face;
            size_t v = i;
            size_t e = edge_id;

            while (!used[v][e]) {
                used[v][e] = true;
                face.push_back(v);

                size_t u = adj[v][e]; // move along edge v -> u

                // find index of v in adj[u] (safe linear search)
                auto it = std::find(adj[u].begin(), adj[u].end(), v);
                if (it == adj[u].end()) {
                    std::cerr << "ERROR: neighbor not found: looking for " << v << " in adj[" << u << "]\n";
                    break;
                }
                size_t pos = static_cast<size_t>(std::distance(adj[u].begin(), it));
                size_t e1 = (pos + 1) % adj[u].size(); // next edge after u->v in CCW order

                v = u;
                e = e1;
            }

            if (face.size() < 1) continue;

            std::reverse(face.begin(), face.end());

            // compute signed area-like value to classify outer/inner
            long double sum = 0;
            Point p1 = vertices[face[0]];
            for (size_t j = 0; j < face.size(); ++j) {
                Point p2 = vertices[face[j]];
                Point p3 = vertices[face[(j + 1) % face.size()]];
                // cast cross to return double/long double
                sum += (long double)((p2 - p1).cross(p3 - p2));
            }

            if (sum <= 0) {
                faces.insert(faces.begin(), face); // outer face first
            } else {
                faces.emplace_back(face);
            }
        }
    }
    if (!faces.empty()) {
        // sort faces by size descending
        std::sort(faces.begin(), faces.end(),
                [](const std::vector<size_t>& a, const std::vector<size_t>& b) {
                    return a.size() > b.size();
                });

        // remove the first one (largest)
        faces.erase(faces.begin());
    } 
    // debug output
    std::cout << "FINAL: Total faces found: " << faces.size() << std::endl;
    for (size_t i = 0; i < faces.size(); i++) {
        std::cout << "Face " << i << ": ";
        for (size_t idx : faces[i]) std::cout << idx << " ";
        std::cout << std::endl;
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


/**
std::vector<std::vector<size_t>> find_faces(std::vector<Point> &vertices, std::vector<std::vector<size_t>> &adj) {
    size_t n = vertices.size();
    
    // DEBUG: Print adjacency list
    std::cout << "DEBUG: Adjacency list:" << std::endl;
    for (size_t i = 0; i < n; i++) {
        std::cout << "Vertex " << i << " (" << vertices[i].x << "," << vertices[i].y << "): ";
        for (size_t neighbor : adj[i]) {
            std::cout << neighbor << " ";
        }
        std::endl;
    }
    
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
            
            // DEBUG: Print each face found
            std::cout << "Found face: ";
            for (size_t idx : face) {
                std::cout << idx << " ";
            }
            std::cout << std::endl;
            
            Point p1 = vertices[face[0]];
            double sum = 0;
            for (size_t j = 0; j < face.size(); ++j) {
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
    
    std::cout << "Total faces found: " << faces.size() << std::endl;
    return faces;
} */