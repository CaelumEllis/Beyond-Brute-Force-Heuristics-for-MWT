#include <cstddef>
#include <functional>
#include "Point.h"
#ifndef POINT_HASH_H
#define POINT_HASH_H


struct PointHash {
		size_t operator()(const Point& p) const {
			std::hash<double> hashFn;
			size_t h1 = hashFn(p.x);
			size_t h2 = hashFn(p.y);
			return h1 ^ (h2 << 1); // combine x and y
		}
	};

#endif