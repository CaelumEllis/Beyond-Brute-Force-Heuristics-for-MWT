using namespace std;

#ifndef POINT_NODE_HASHER_H
#define POINT_NODE_HASHER_H
#include "Point.h"
#include "PointHash.h"
#include <vector>
#include <unordered_map>
namespace PointNodeHasher {
  using PointPairType = std::vector<std::pair<double, double>>;

	std::unordered_map<Point, int, PointHash> mapCoordinates(PointPairType coordinates) {
    std::unordered_map<Point, int, PointHash> pointNodeTable;
    size_t nodes = 0;
		for (auto c : coordinates) {
			Point p = Point(c.first, c.second);
			pointNodeTable[p] = nodes;
			nodes++;
		}
		return pointNodeTable;
	}
}
#endif