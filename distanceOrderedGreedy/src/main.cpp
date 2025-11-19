
#include <iostream>
#include "triangulate.h"
#include "geometry.h"
#include "load_coordinates.h"
int main(int argc, char** argv) {
    /* std::vector<Point> P = {
        {0,0}, {5,0}, {4,3}, {1,4}, {2,2}, {3,1}
    }; */

      if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <dataset_file>\n";
        return 1;
    }
    
    std::string filename = argv[1];
    auto coordinates = loadCoordinatesFromFile(filename);
    std::vector<Point> P;
    P.reserve(coordinates.size());
   for (const auto &p : coordinates) {
    P.push_back(Point(p.x, p.y));
}
    auto T = DOGT(P);
    std::cout << "FINAL: " << T << "\n";
    /* for (auto &tr : T) {
        std::cout << "(" << tr.a << "," << tr.b << "," << tr.c << ")\n";
    } */
}
