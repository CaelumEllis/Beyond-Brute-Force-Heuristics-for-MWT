
#include <iostream>
#include "triangulate.hpp"

int main() {
    std::vector<Point> P = {
        {0,0}, {5,0}, {4,3}, {1,4}, {2,2}, {3,1}
    };

    auto T = DOGT(P);

    for (auto &tr : T) {
        std::cout << "(" << tr.a << "," << tr.b << "," << tr.c << ")\n";
    }
}
