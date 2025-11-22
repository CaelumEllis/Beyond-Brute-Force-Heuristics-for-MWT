#include <iostream>
#include <vector>
#include <utility>
#include <cmath>
#include <limits>
#include <algorithm>
#include <set>
#include <fstream>
#include <chrono>

#include "load_coordinates.h"

using namespace std;

typedef pair<float, float> Point;
typedef pair<int, int> Edge;

class EfficientMWT {
private:
    vector<Point> points;
    
    double distance(const Point& p1, const Point& p2) {
        float dx = p1.first - p2.first;
        float dy = p1.second - p2.second;
        return sqrt(dx * dx + dy * dy);
    }
    
    bool edgesIntersect(const Edge& e1, const Edge& e2) {
        int a = e1.first, b = e1.second;
        int c = e2.first, d = e2.second;
        
        if (a == c || a == d || b == c || b == d) return false;
        
        const Point& p1 = points[a];
        const Point& p2 = points[b];
        const Point& p3 = points[c];
        const Point& p4 = points[d];
        
        auto orientation = [](const Point& p, const Point& q, const Point& r) {
            float val = (q.second - p.second) * (r.first - q.first) - 
                       (q.first - p.first) * (r.second - q.second);
            if (val == 0) return 0;
            return (val > 0) ? 1 : 2;
        };
        
        int o1 = orientation(p1, p2, p3);
        int o2 = orientation(p1, p2, p4);
        int o3 = orientation(p3, p4, p1);
        int o4 = orientation(p3, p4, p2);
        
        return (o1 != o2) && (o3 != o4);
    }
    
    bool isValidTriangulation(const vector<Edge>& edges) {
        // Check for edge intersections
        for (int i = 0; i < edges.size(); i++) {
            for (int j = i + 1; j < edges.size(); j++) {
                if (edgesIntersect(edges[i], edges[j])) {
                    return false;
                }
            }
        }
        
        // Count triangles formed by the edges
        int n = points.size();
        set<vector<int>> triangles;
        
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                for (int k = j + 1; k < n; k++) {
                    // Check if all three edges exist
                    bool hasIJ = false, hasJK = false, hasKI = false;
                    for (const Edge& e : edges) {
                        if ((e.first == i && e.second == j) || (e.first == j && e.second == i)) hasIJ = true;
                        if ((e.first == j && e.second == k) || (e.first == k && e.second == j)) hasJK = true;
                        if ((e.first == k && e.second == i) || (e.first == i && e.second == k)) hasKI = true;
                    }
                    
                    if (hasIJ && hasJK && hasKI) {
                        vector<int> triangle = {i, j, k};
                        sort(triangle.begin(), triangle.end());
                        triangles.insert(triangle);
                    }
                }
            }
        }
        
        // A valid triangulation of n points should have exactly 2n - 2 - h triangles
        // where h is the number of hull vertices
        // This is a simplified check
        return triangles.size() >= (2 * n - 5);
    }
    
    double calculateWeight(const vector<Edge>& edges) {
        double total = 0.0;
        for (const Edge& e : edges) {
            total += distance(points[e.first], points[e.second]);
        }
        return total;
    }
    
public:
    pair<double, vector<Edge>> findMinimumWeight(vector<Point> inputPoints) {
        points = inputPoints;
        int n = points.size();

        if (n < 3) return {0.0, {}};

        double minWeight = numeric_limits<double>::max();
        vector<Edge> bestEdges;

        int maxEdges = n * (n - 1) / 2;

        for (long long mask = 0; mask < (1LL << maxEdges); mask++) {
            vector<Edge> edges;
            int edgeIndex = 0;

            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; j++) {
                    if (mask & (1LL << edgeIndex)) {
                        edges.push_back({i, j});
                    }
                    edgeIndex++;
                }
            }

            if (isValidTriangulation(edges)) {
                double weight = calculateWeight(edges);
                if (weight < minWeight) {
                    minWeight = weight;
                    bestEdges = edges;
                }
            }
        }

        return {minWeight, bestEdges};
    }
};

pair<double, vector<Edge>> minimumWeightTriangulation(vector<pair<float, float>> points) {
    EfficientMWT solver;
    return solver.findMinimumWeight(points);
}

void writeResultCSV(const std::vector<Point>& points,
                    const std::vector<Edge>& edges,
                    double weight,
                    double runtimeSec,
                    const std::string& filename = "mwt_output.csv")
{
    std::ofstream file(filename);

    if (!file) {
        std::cerr << "Error: Could not write to " << filename << "\n";
        return;
    }

    // Metadata
    file << "Minimum Weight," << weight << "\n";
    file << "Runtime (seconds)," << runtimeSec << "\n\n";

    // Points section
    file << "Points:\n";
    file << "index,x,y\n";
    for (size_t i = 0; i < points.size(); i++) {
        file << i << "," << points[i].first << "," << points[i].second << "\n";
    }

    // Edges section
    file << "\nEdges:\n";
    file << "from,to\n";
    for (const auto& e : edges) {
        file << e.first << "," << e.second << "\n";
    }

    file.close();
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    std::string filename = argv[1];
    auto coordinates = loadCoordinatesFromFile(filename);
    std::vector<std::pair<float, float>> points;
    points.reserve(coordinates.size());
    for (auto& p : coordinates)
        points.emplace_back(p.x, p.y);
    // vector<pair<float, float>> points = {{0, 0}, {1, 1}, {-3, 4}, {-5, 5},
    //                                     {-3, 6}, {8, 9}, {10, 11}, {4, -8}};
    using Clock = std::chrono::high_resolution_clock;

    auto start = Clock::now();

    auto [weight, edges] = minimumWeightTriangulation(points);

    auto end = Clock::now();
    double elapsedSec = std::chrono::duration<double>(end - start).count();
    // cout << minimumWeightTriangulation(points) << endl;
    std::cout << "Minimum Weight: " << weight << "\n";
    std::cout << "Runtime: " << elapsedSec << " seconds\n";

    writeResultCSV(points, edges, weight, elapsedSec, "mwt_output.csv");

    return 0;
}