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

struct TriangulationResult {
    double cost;
    vector<pair<size_t, size_t>> edges; 
};

void printEdges(const vector<pair<size_t, size_t>> &edges) {
    std::cout << "EDGES " << "\n";
    for (auto &e : edges) {
        std::cout << e.first << "-" << e.second << "\n";
    }
}
// Helper to extract edges from split table (using pointer to first element)
void extractEdges(size_t* split, size_t n, size_t i, size_t j, vector<pair<size_t, size_t>>& edges) {
    if (j < i+2) return;
    
    size_t k = split[i * n + j];  // Access 2D array as 1D
    edges.push_back({i, k});
    edges.push_back({k, j});
    
    extractEdges(split, n, i, k, edges);
    extractEdges(split, n, k, j, edges);
}

TriangulationResult mTC(vector<Point> points, size_t n)
{
   // There must be at least 3 points to form a triangle
   if (n < 3)
      return {0.0, {}};
   
   std::cout << "n = " << n << "\n";
   
   // table to store results of subproblems.  table[i][j] stores cost of
   // triangulation of points from i to j.
   double table[n][n];
   
   // split[i][j] stores the k value that gave optimal triangulation for points i to j
   size_t split[n][n];

   // Fill table using above recursive formula. Note that the table
   // is filled in diagonal fashion i.e., from diagonal elements to
   // table[0][n-1] which is the result.
   for (size_t gap = 0; gap < n; gap++)
   {
      for (size_t i = 0, j = gap; j < n; i++, j++)
      {
          if (j < i+2) {
              table[i][j] = 0.0;
              split[i][j] = 0;  // No split needed
              std::cout << "zeroed\n";
          } else {
              table[i][j] = numeric_limits<double>::max();
              split[i][j] = 0;
              
              for (size_t k = i+1; k < j; k++)
              {
                  double val = table[i][k] + table[k][j] + cost(points,i,j,k);
                  if (table[i][j] > val) {
                      table[i][j] = val;
                      split[i][j] = k;  // Remember which k was optimal
                  }
              }
              std::cout << "new " << i << " " << j << "\n";
          }
      }
   }
   
   // Backtrack to find all edges
   vector<pair<size_t, size_t>> edges;
   extractEdges(&split[0][0], n, 0, n-1, edges);
   
   // Add the polygon boundary edges (these are implicit in the triangulation)
   for (size_t i = 0; i < n-1; i++) {
       edges.push_back({i, i+1});
   }
   edges.push_back({n-1, 0});  // Close the polygon
   
   return {table[0][n-1], edges};
}


} // namespace PolygonalMWT

#endif


