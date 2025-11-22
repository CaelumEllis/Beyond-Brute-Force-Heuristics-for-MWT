
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

#include "GrahamScan.h"
#include "PolygonalMWT.h"
#include "load_coordinates.h"
#include "AdjListGraph.h"
#include "FaceFinder.h"
#include "PointHash.h"
#include "PointNodeHasher.h"
using namespace std;


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    std::string filename = argv[1];
    using PointPairType = std::vector<std::pair<double, double>>;
    auto coordinates = loadCoordinatesFromFile(filename);
    auto start = std::chrono::high_resolution_clock::now();
    // Convert into std::pair<double,double> format
    PointPairType pointPairs;
    pointPairs.reserve(coordinates.size());

    for (const auto& c : coordinates) {
        pointPairs.emplace_back(c.x, c.y);
    }

  /*   // this is a map corresponding each coordinate/point to a node 'number'
    std::unordered_map<Point, int, PointHash> pointHashMap = PointNodeHasher::mapCoordinates(pointPairs); */
    // create a complete graph first (adjacency list)
    AdjListGraph complete(pointPairs);
    

    /**
     * finding hull
     * s1/ find hull w.r.t points
     * s2/ convert into our nodes & create adj list
     */
    PointPairType convexHullPoints = GrahamScan::findConvexHull(pointPairs);
    std::vector<size_t> convexHullNodes(convexHullPoints.size());
    std::transform(convexHullPoints.begin(), convexHullPoints.end(), convexHullNodes.begin(),
    [&](const auto &p) { return complete.getNodeForPoint(p); });
   
    // todo replace this with a better way of finding the convex hull's weight
    double convexHullWeight = GrahamScan::findConvexWeight(convexHullPoints);
    // std::cout << "CONVEX HULL WEIGHT: " << convexHullWeight << "\n";
    // mst: kruskals from graph. we can assume every node is included lol :3 yaaay msts
    std::vector<std::vector<size_t>> mstEdges = complete.kruskalMST();

    // adds edges to complete polygons (accounting for leaf nodes not connected to the hull)
    // comment this out if unnecessary/testing old vers.
    complete.fixTUC(mstEdges, convexHullNodes);
    double MSTWeight = complete.computeAdjListWeight(mstEdges);
    // iterate through all leaf nodes, find each leaf node that is NOT in the convex hull nodes

    // add edges of mst and hull tgt, and process into a graph (with adj list, points) (can remove weights here)
    std::vector<std::vector<size_t>> edgeSumList = complete.mergeAdjListAndConvex(mstEdges, convexHullNodes);

    // the edge weight to add to the final is actually here rip
    double totalWeight = complete.computeAdjListWeight(edgeSumList);
    //std::cout << "COMBINED WEIGHT : " << totalWeight << "\n";
    // run facefinder algo (needs an input of all points + adj list)
    auto list = complete.getPoints();
    std::vector<std::vector<size_t>> faceList = FaceFinder::find_faces(list, edgeSumList);
     //std::cout << "NO. OF POLYGONS: " << faceList.size() << "\n";
    // for each face, run mwt polygonisation algo 
    /**
    need to remove the boundary per polygon and add in the convex weight + mst weight manually
     */
    double currWeight = 0;
    for (auto v : faceList) {
        std::vector<Point> facePointList(v.size());
        std::transform(v.begin(), v.end(),facePointList.begin(),
        [&](const auto &p) { return complete.getPointForNode(p); });
        
        auto totalDiag = PolygonalMWT::mTC(facePointList, facePointList.size());
    
        //std::cout << "TOTAL DIAG: " << totalDiag << "\n";
        auto boundaryWeight = FaceFinder::computePointBoundaryWeight(facePointList);
       
        //std::cout<< "WEIGHT OF BOUNDARY: " << boundaryWeight;
        auto internalDiag = ((totalDiag - boundaryWeight) / 2);
        //std::cout << "INTERNAL DIAG: " << internalDiag << "\n";
        currWeight += internalDiag;
    }
    
    currWeight += totalWeight;

    //std::cout << "Final MWT approx by MST polygonisation is " << currWeight << std::endl;
    auto end = std::chrono::high_resolution_clock::now();
    double runtime_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // Print final line for auto tester:
    std::cout << "RESULT," << currWeight << "," << runtime_ms << "\n";
    return 0;
}


int pairToNode(const std::pair<double,double>& pr, const AdjListGraph& graph) {
    Point p(pr.first, pr.second);
    return graph.getNodeForPoint(p);  // uses your hash table
}



/**
 * notes to self (delete before presentation)
 * convex hull
 * mst
 * combine (set of edges probs to prevent double count)
 * somehow find every planar face???
 * put every planar face into the polygonal mst dp algo
 */ 