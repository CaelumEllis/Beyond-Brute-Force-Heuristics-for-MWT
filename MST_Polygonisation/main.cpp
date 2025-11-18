
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "includes/GrahamScan.h"
#include "includes/PolygonalMWT.h"
#include "load_coordinates.h"
#include "GraphState.h"
#include "includes/AdjListGraph.h"
#include "includes/PointNodeHasher.h"
#include "includes/FaceFinder.h"
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    std::string filename = argv[1];
    using PointPairType = std::vector<std::pair<double, double>>;
    auto coordinates = loadCoordinatesFromFile(filename);

    // Convert into std::pair<double,double> format
    PointPairType pointPairs;
    pointPairs.reserve(coordinates.size());

    for (const auto& c : coordinates) {
        pointPairs.emplace_back(c.x, c.y);
    }

    // this is a map corresponding each coordinate/point to a node 'number'
    std::unordered_map<Point, int, PointHash> pointHashMap = PointNodeHasher::mapCoordinates(pointPairs);
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
    // find edges
    // todo replace this with a better way of finding the convex hull's weight
    double convexHullWeight = GrahamScan::findConvexWeight(convexHullPoints);

    // mst: kruskals from graph. we can assume every node is included lol :3 yaaay msts
    std::vector<std::vector<size_t>> mstEdges = complete.kruskalMST();
    double MSTWeight = complete.computeAdjListWeight(mstEdges);
    // add edges of mst and hull tgt, and process into a graph (with adj list, points) (can remove weights here)

    std::vector<std::vector<size_t>> edgeSumList = complete.mergeAdjListAndConvex(mstEdges, convexHullNodes);
    // run facefinder algo (needs an input of all points + adj list)
    std::vector<std::vector<size_t>> faceList = FaceFinder::find_faces(complete.getPoints(), edgeSumList);

    // for each face, run mwt polygonisation algo 
    /**
    need to remove the boundary per polygon and add in the convex weight + mst weight manually
     */
    double currWeight = 0;
    for (auto v : faceList) {
        std::vector<Point> facePointList(v.size());
        std::transform(v.begin(), v.end(),facePointList.begin(),
        [&](const auto &p) { return complete.getPointForNode(p); });
       currWeight += PolygonalMWT::mTC(facePointList, 0, facePointList.size() - 1);
       // remove boundary after
       currWeight -= FaceFinder::computePointBoundaryWeight(facePointList);
    }
    
    currWeight += convexHullWeight;
    currWeight += MSTWeight;

    std::cout << "Final MWT approx by MST polygonisation is " << currWeight << std::endl;
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