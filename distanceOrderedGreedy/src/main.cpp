#include <iostream>
#include <chrono>
#include "triangulate.h"
#include "geometry.h"
#include "load_coordinates.h"

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }

    std::string filename = argv[1];
    auto coordinates = loadCoordinatesFromFile(filename);

    std::vector<Point> P;
    P.reserve(coordinates.size());
    for (const auto& p : coordinates)
        P.emplace_back(p.x, p.y);

    if (P.size() < 3) {
        std::cout << "RESULT,0,0\n";
        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto T = DOGT(P);   // This algorithm already returns a numeric triangulation weight
    auto end = std::chrono::high_resolution_clock::now();

    double runtime_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // T already is the minimum triangulation weight for this heuristic algorithm
    std::cout << "RESULT," << T << "," << runtime_ms << "\n";
    return 0;
}
