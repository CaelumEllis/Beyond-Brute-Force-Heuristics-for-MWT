//
// Created by Caelum Ellis on 16/11/2025.
//

#ifndef CANDIDATEEDGEFILTER_H
#define CANDIDATEEDGEFILTER_H

#include <vector>
#include "GraphState.h"



  // CandidateEdgeFilter builds a subset of edges that are likely to be
  // valid candidates for edge flipping, due to being long.
  //
  // Heuristic:
  //   - For each vertex, take the k (2-3) longest incident edges.
  //   - Globally, also take the top p fraction (1/3) of longest edges.
  //   - Return the union of these sets with duplicates removed.

class CandidateEdgeFilter {
public:

      // Build candidate edge set from a Delaunay-based GraphState.
      //  Args:
      //      gs                - The underlying graph (points, edges, triangles).
      //      perVertex         - How many longest edges to keep per vertex (default 3).
      //      globalFraction    - Fraction of global longest edges to keep (default 1/3).
      //                          Clamped to [0,1]. If >0 but <1/|E|, at least 1 edge kept.
      //  Returns:
      //      Vector            - contains unique Edge objects (same (u,v,weight) as in gs.edges).

    static std::vector<Edge> buildCandidateSet(
        const GraphState& gs,
        int    perVertex      = 1,
        double globalFraction = 1.0 / 5.0
    );
};
#endif // CANDIDATEEDGEFILTER_H
