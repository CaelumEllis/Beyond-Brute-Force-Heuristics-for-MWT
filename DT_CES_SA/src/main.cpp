
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "load_coordinates.h"
#include "CandidateEdgeFilter.h"
#include "SimulatedAnnealing.h"
#include "DelaunayWrapper.h"
#include "GraphState.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    std::string filename = argv[1];

    auto coordinates = loadCoordinatesFromFile(filename);

    // Convert into std::pair<double,double> format
    std::vector<std::pair<double,double>> pointPairs;
    pointPairs.reserve(coordinates.size());

    for (const auto& c : coordinates) {
        pointPairs.emplace_back(c.x, c.y);
    }

    // returns DTResult which is the set of edges and triangles of the Delaunay triangulation computed by SimpleDelaunay
    // Delaunay triangulation computed by SimpleDelaunay in average O(nlogn) runtime
    auto DTResult = DelaunayWrapper::translateOutput(pointPairs);

    // put triangulation and edges into a GraphState
    GraphState gs(DTResult, pointPairs);

    // ---- Debug print ----
    std::cout << "Delaunay Graph built.\n";
    std::cout << "Points: " << gs.points.size() << "\n";
    std::cout << "Edges: " << gs.edges.size() << "\n";
    std::cout << "Triangles: " << gs.triangles.size() << "\n\n";

    std::cout << "Edges:\n";
    for (auto &e : gs.edges) {
        std::cout << "(" << e.u << "," << e.v << ") len=" << e.weight << "\n";
    }

    return 0;
}

// int main(int argc, char** argv) {
//     if (argc < 2) {
//         std::cerr << "Usage of " << argv[0] << " requires <test_datasets/file> where file is set of coordinates of valid format\n";
//         return 1;
//     }
//
//     auto coordinates = loadCoordinatesFromFile(argv[1]);
//
//     DelaunayWrapper Dw;
//     auto coordinates = Dw.translateOutput(coor);
//
//     auto candidateEdges = CandidateEdgeFilter::filter(coordinates, dt.getEdges());
//
//     SimulatedAnnealing sa;
//     auto optimisedEdges = sa.optimize(coordinates, candidateEdges);
//
//     std::cout << "Triangulation Generated Successfully.\n";
// }



//////////////////////////////////////////////////////////////////////////////////////
// Only Use to confirm that the data is loaded correctly into a coordinate vector   //
//////////////////////////////////////////////////////////////////////////////////////
// int main(int argc, char** argv) {
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
//         return 1;
//     }
//
//     std::string filename = argv[1];
//
//     try {
//         auto coordinates = loadCoordinatesFromFile(filename);
//
//         std::cout << "Loaded " << coordinates.size() << " coordinates:\n";
//         for (const auto& p : coordinates) {
//             std::cout << "(" << p.x << ", " << p.y << ")\n";
//         }
//
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << "\n";
//         return 1;
//     }
//
//     return 0;
// }
//////////////////////////////////////////////////////////////////////////////////////