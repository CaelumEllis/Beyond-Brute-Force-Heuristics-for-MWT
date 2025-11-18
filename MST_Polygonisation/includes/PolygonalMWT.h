#ifndef POLYGONAL_MWT_H
#define POLYGONAL_MWT__H
#include "Point.h"
#include <vector>

#include <iostream>
#include "Point.h"
#include <cmath>
#include <limits>
using namespace std;

/**
 * also from geeks4geeks shamelessly,
 * known polygonal dp mwt calculation
 */

namespace PolygonalMWT {
	// A utility function to find distance between two points in a plane
	double dist(Point p1, Point p2)
	{
		return sqrt((p1.x - p2.x)*(p1.x - p2.x) +
					(p1.y - p2.y)*(p1.y - p2.y));
	}

	// A utility function to find cost of a triangle. The cost is considered
	// as perimeter (sum of lengths of all edges) of the triangle
	double cost(std::vector<Point> points, int i, int j, int k)
	{
		Point p1 = points.at(i), p2 = points.at(j), p3 = points.at(k);
		return dist(p1, p2) + dist(p2, p3) + dist(p3, p1);
	}

	// A recursive function to find minimum cost of polygon triangulation
	// The polygon is represented by points[i..j].
	double mTC(std::vector<Point> points, int i, int j)
	{
		// There must be at least three points between i and j
		// (including i and j)
		if (j < i+2)
			return 0;

		// Initialize result as infinite
		double res = std::numeric_limits<double>::max();

		// Find minimum triangulation by considering all
		for (int k=i+1; k<j; k++)
				res = std::min(res, (mTC(points, i, k) + mTC(points, k, j) +
								cost(points, i, k, j)));
		return res;
	}
}
#endif