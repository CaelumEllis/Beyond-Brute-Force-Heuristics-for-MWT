
#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "load_coordinates.h"
#include "CandidateEdgeFilter.h"
#include "SimulatedAnnealing.h"
#include "DelaunayWrapper.h"
#include "GraphState.h"
#include "FlipCriteria.h"

using Clock = std::chrono::high_resolution_clock;

int main(int argc, char** argv) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    std::string filename = argv[1];

    // Load Points/vectos from file
    auto coordinates = loadCoordinatesFromFile(filename);
    std::vector<std::pair<double,double>> points;
    points.reserve(coordinates.size());
    for (auto &p : coordinates)
        points.emplace_back(p.x, p.y);

    // Delaunay Triangulation Time
    auto t1_start = Clock::now();
    auto dt = DelaunayWrapper::translateOutput(points);
    GraphState gs(dt, points);
    auto t1_end = Clock::now();
    long long dtTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t1_end - t1_start).count();



    // Candidate Edge List Build Time
    auto t2_start = Clock::now();
    auto candidateEdges = CandidateEdgeFilter::buildCandidateSet(gs);
    auto t2_end = Clock::now();
    long long candidateTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t2_end - t2_start).count();

    // Simulated Annealing + Greedy Optimisation
    SimulatedAnnealing sa;

    // This function tunes the initial temperature and cooling rate
    // based on the number of candidate edges available
    sa.configureDynamic(gs, candidateEdges);
    double initialWeight = sa.computeWeight(gs);

    auto t3_start = Clock::now();
    // run simulated annealing then greedy optimisation
    sa.run(gs);
    sa.greedyImprove(gs);

    auto t3_end = Clock::now();

    long long saTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(t3_end - t3_start).count();

    // Test Metrics
    double finalWeight = sa.computeWeight(gs);
    double improvement = (initialWeight - finalWeight) / initialWeight * 100.0;

    /////////////////////////////////////////
    // Summary Output
    /////////////////////////////////////////
    // std::cout << "\n===== Run Summary =====\n";
    // std::cout << "File: " << filename << "\n";
    // std::cout << "Points: " << gs.points.size() << "\n";
    // std::cout << "Edges: " << gs.edges.size() << "\n\n";
    //
    // std::cout << "DT Time (ms):        " << dtTimeMs << "\n";
    // std::cout << "Candidate Time (ms): " << candidateTimeMs << "\n";
    // std::cout << "SA Time (ms):        " << saTimeMs << "\n\n";
    //
    // std::cout << "Initial Weight: " << initialWeight << "\n";
    // std::cout << "Final Weight:   " << finalWeight << "\n";
    // std::cout << "Improvement:    " << improvement << "%\n";
    // std::cout << "Accepted Flips: " << sa.totalAccepted << "\n";

    ///////////////////////////////////////////
    // Export final triangulation for plotting
    ///////////////////////////////////////////
    // std::string outPlotFile = filename + "_generated_triangulation.csv";
    // std::ofstream plotOut(outPlotFile);
    //
    // plotOut << "x,y\n";
    // for (const auto& p : gs.points)
    //     plotOut << p.first << "," << p.second << "\n";
    //
    // plotOut << "\n#EDGES u,v\n";
    // for (const auto& e : gs.edges)
    //     plotOut << e.u << "," << e.v << "\n";
    //
    // plotOut.close();
    //
    // std::cout << "PLOT_OUTPUT," << outPlotFile << "\n";

    /////////////////////////////////////////
    // Output for CSV Data Collection
    /////////////////////////////////////////

    // extra derived metrics

   // std::cout << "RESULT,"
   // << filename << ","
   // << gs.points.size() << ","     // number of points
   //<< gs.edges.size() << ","      // edge count
   // << dtTimeMs << ","             // time for initial Delaunay
   // << candidateTimeMs << ","      // candidate filter time
   // << saTimeMs << ","             // simulated annealing time
   // << initialWeight << ","        // weight before optimisation
   // << finalWeight << ","          // weight after optimisation
   // << improvement << ","          // improvement percentage
   // << sa.totalAccepted            // number of accepted flips
    //<< "\n";
    std::cout << "RESULT," << finalWeight << "," << saTimeMs << "\n";


    return 0;
}





// //////////////////////////////////////////////////////////////////////////////////////
// //   Used to trial different parameters for simulated annealing and print the results
// //////////////////////////////////////////////////////////////////////////////////////
// int main(int argc, char** argv) {
//
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
//         return 1;
//     }
//
//     std::string filename = argv[1];
//
//     // Load points/vertices from file
//     auto coordinates = loadCoordinatesFromFile(filename);
//     std::vector<std::pair<double,double>> pointPairs;
//     pointPairs.reserve(coordinates.size());
//
//     for (const auto& c : coordinates)
//         pointPairs.emplace_back(c.x, c.y);
//
//     //  Returns Delaunay triangulation in DTResult format
//     auto dt = DelaunayWrapper::translateOutput(pointPairs);
//     GraphState gs(dt, pointPairs);
//
//     std::cout << "\n=====================================\n";
//     std::cout << "   Initial Delaunay Triangulation\n";
//     std::cout << "=====================================\n";
//     std::cout << "Points: " << gs.points.size() << "\n";
//     std::cout << "Edges:  " << gs.edges.size()  << "\n";
//     // for (const auto& e : gs.edges)
//     //     std::cout << "(" << e.u << "," << e.v << ") len=" << e.weight << "\n";
//
//     // Generates initial candidate edge list
//     auto candidateEdges = CandidateEdgeFilter::buildCandidateSet(gs);
//
//     double cnt = 0;
//     for (auto& e : candidateEdges) {
//         if (FlipCriteria::isFlipLegal(gs, e.u, e.v).legal)
//             cnt++;
//     }
//
//     std::cout << "\nInitial Candidate Edge Count: " << candidateEdges.size() << "\n";
//     std::cout << "Legal flips available: " << cnt << "\n";
//
//     std::cout << "\n=====================================\n";
//     std::cout << "   Running Simulated Annealing...\n";
//     std::cout << "=====================================\n";
//
//     SimulatedAnnealing sa;
//     sa.initialTemperature = 0.30;
//     sa.coolingRate        = 0.99965;
//     sa.minTemperature     = 1e-6;
//     sa.maxIterations      = 150000;
//
//     std::cout << "Initial Weight: " << sa.computeWeight(gs) << "\n";
//     sa.run(gs);
//     // sa.greedyImprove(gs);
//     std::cout << "Final Weight:   " << sa.computeWeight(gs) << "\n";
//     // ---- Print Final Output ----
//     // std::cout << "\n=====================================\n";
//     // std::cout << "   Final Graph After Annealing\n";
//     // std::cout << "=====================================\n";
//     //
//     // for (const auto& e : gs.edges)
//     //     std::cout << "(" << e.u << "," << e.v << ") len=" << e.weight << "\n";
//
//     std::cout << "\Done.\n";
//     return 0;
// }

// //////////////////////////////////////////////////////////////////////////////////////
// // Used to print the edges returned by SimpleDelaunay and the triangulation generated by it//
// //////////////////////////////////////////////////////////////////////////////////////
// int main(int argc, char** argv) {
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
//         return 1;
//     }
//     std::string filename = argv[1];
//
//     auto coordinates = loadCoordinatesFromFile(filename);
//
//     // Convert into std::pair<double,double> format
//     std::vector<std::pair<double,double>> pointPairs;
//     pointPairs.reserve(coordinates.size());
//
//     for (const auto& c : coordinates) {
//         pointPairs.emplace_back(c.x, c.y);
//     }
//
//     // returns DTResult which is the set of edges and triangles of the Delaunay triangulation computed by SimpleDelaunay
//     // Delaunay triangulation computed by SimpleDelaunay in average O(nlogn) runtime
//     auto DTResult = DelaunayWrapper::translateOutput(pointPairs);
//
//     // put triangulation and edges into a GraphState
//     GraphState gs(DTResult, pointPairs);
//
//     // ---- Debug print ----
//     std::cout << "Delaunay Graph built.\n";
//     std::cout << "Points: " << gs.points.size() << "\n";
//     std::cout << "Edges: " << gs.edges.size() << "\n";
//     std::cout << "Triangles: " << gs.triangles.size() << "\n\n";
//
//     std::cout << "Edges:\n";
//     for (auto &e : gs.edges) {
//         std::cout << "(" << e.u << "," << e.v << ") len=" << e.weight << "\n";
//     }
//
//     return 0;
// }


// ////////////////////////////////////////////////////////////////////////////////////
//  Only Use to confirm that the data is loaded correctly into a coordinate vector   //
// ////////////////////////////////////////////////////////////////////////////////////
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