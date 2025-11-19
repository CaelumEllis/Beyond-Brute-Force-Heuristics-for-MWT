#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cmath>
#include <cfloat>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std;

// Euclidean distance
double dist(const pair<float, float>& a, const pair<float, float>& b) {
    double dx = a.first - b.first;
    double dy = a.second - b.second;
    return sqrt(dx * dx + dy * dy);
}

// Triangle perimeter (weight)
double triangleWeight(const vector<pair<float, float>>& pts, int i, int j, int k) {
    return dist(pts[i], pts[j]) + dist(pts[j], pts[k]) + dist(pts[k], pts[i]);
}

// Recursive DP for minimum-weight triangulation
double minTriangulation(const vector<pair<float, float>>& pts, int i, int j,
                        vector<vector<double>>& memo) {
    if (j <= i + 1)
        return 0.0;

    if (memo[i][j] >= 0.0)
        return memo[i][j];

    double best = DBL_MAX;

    for (int k = i + 1; k < j; ++k) {
        double cost = minTriangulation(pts, i, k, memo)
                    + minTriangulation(pts, k, j, memo)
                    + triangleWeight(pts, i, k, j);
        best = min(best, cost);
    }

    memo[i][j] = best;
    return best;
}

// Wrapper
double minimumWeightTriangulation(const vector<pair<float, float>>& pts) {
    int n = pts.size();
    vector<vector<double>> memo(n, vector<double>(n, -1.0));
    return minTriangulation(pts, 0, n - 1, memo);
}

// Reads points from file in format: x,y (one per line)
vector<pair<float, float>> readPointsFromFile(const string& filename) {
    vector<pair<float, float>> pts;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot open file " << filename << "\n";
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        replace(line.begin(), line.end(), ',', ' '); // replace comma with space
        stringstream ss(line);
        float x, y;
        if (ss >> x >> y)
            pts.emplace_back(x, y);
    }

    file.close();
    return pts;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <dataset_file> [runs]\n";
        return 1;
    }

    string filename = argv[1];

    vector<pair<float, float>> pts = readPointsFromFile(filename);

    if (pts.size() < 3) {
        cerr << "Error: need at least 3 points to form a triangulation.\n";
        return 1;
    }

    auto start = chrono::high_resolution_clock::now();
    double weight = minimumWeightTriangulation(pts);
    auto end = chrono::high_resolution_clock::now();

    double runtime_ms = chrono::duration<double, milli>(end - start).count();

    cout << fixed << setprecision(6);
    cout << "RESULT," << weight << "," << runtime_ms << "\n";

    return 0;
}