#ifndef POLYGONAL_MWT_H
#define POLYGONAL_MWT_H
#include "Point.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
using namespace std;

/**
 * Original DP MWT calculation, with splits added (vector version)
 * Adapted from geeks4geeks
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
double cost(vector<Point> points, size_t i, size_t j, size_t k)
{
    Point p1 = points[i], p2 = points[j], p3 = points[k];
    return dist(p1, p2) + dist(p2, p3) + dist(p3, p1);
}

double mTC(vector<Point> points, size_t n)
{
   if (n < 3)
      return 0;
   
   // dp table
   double table[n][n];
   // edges
   size_t split[n][n];

   for (size_t gap = 0; gap < n; gap++)
   {
      for (size_t i = 0, j = gap; j < n; i++, j++)
      {
          if (j < i+2) {
              table[i][j] = 0.0;
              split[i][j] = 0; 
          } else {
              table[i][j] = numeric_limits<double>::max();
              split[i][j] = 0;
              
              for (size_t k = i+1; k < j; k++)
              {
                  double val = table[i][k] + table[k][j] + cost(points,i,j,k);
                  if (table[i][j] > val) {
                      table[i][j] = val;
                      split[i][j] = k; 
                  }
              }
          }
      }
   }
 
   
   return table[0][n-1];
}


} // namespace PolygonalMWT

#endif


