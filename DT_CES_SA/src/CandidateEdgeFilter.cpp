//
// Created by Caelum Ellis on 16/11/2025.
//
#include "EdgeKey.h"
#include "CandidateEdgeFilter.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>

using EdgePtr = const Edge*;

std::vector<Edge> CandidateEdgeFilter::buildCandidateSet(
    const GraphState& gs,
    int perVertex,
    double globalFraction
) {
    std::vector<Edge> result;
    if (gs.edges.empty()) {
        return result;
    }

    // Clamp parameters to safe ranges
    if (perVertex < 0)       perVertex = 0;
    // Theoretically 6 is average in a maximised triangulation that is planar
    if (perVertex > 5)       perVertex = 5;
    if (globalFraction < 0)  globalFraction = 0;
    if (globalFraction > 1)  globalFraction = 1;

    // -------------------------------------------------
    //  Build EdgeKey - Allows for Quick lookup of Edges
    //  e.g edgeLookup[EdgeKey(u,v)] = &e;
    // -------------------------------------------------
    std::unordered_map<EdgeKey, EdgePtr, EdgeKeyHash> edgeLookup;
    edgeLookup.reserve(gs.edges.size());

    for (const auto& e : gs.edges) {
        EdgeKey key(e.u, e.v);
        edgeLookup.emplace(key, &e);
    }

    // -------------------------------------------------
    //  Build incident edge lists per vertex
    //  (so we can pick the longest k per vertex)
    // -------------------------------------------------
    std::vector<std::vector<EdgePtr>> incident(gs.points.size());

    for (const auto& e : gs.edges) {
        EdgePtr ep = &e;

        if (e.u >= 0 && static_cast<size_t>(e.u) < incident.size()) {
            incident[e.u].push_back(ep);
        }
        if (e.v >= 0 && static_cast<size_t>(e.v) < incident.size()) {
            incident[e.v].push_back(ep);
        }
    }

    // -------------------------------------------------
    // Collect candidate edges as EdgeKey set
    // -------------------------------------------------
    std::unordered_set<EdgeKey, EdgeKeyHash> candidateKeys;
    candidateKeys.reserve(gs.edges.size());

    // Longest per vertex
    if (perVertex > 0) {
        for (size_t v = 0; v < incident.size(); ++v) {
            auto& vec = incident[v];
            if (vec.empty()) continue;

            // sort descending by edge length
            std::sort(vec.begin(), vec.end(),
                      [](EdgePtr a, EdgePtr b) {
                          return a->weight > b->weight;
                      });
            const int take = std::min<int>(perVertex,
                                    static_cast<int>(vec.size()));
            for (int i = 0; i < take; ++i) {
                const Edge* e = vec[i];
                candidateKeys.insert(EdgeKey(e->u, e->v));
            }
        }
    }

    // Global top fraction of edges (by length)
    if (globalFraction > 0.0) {
        std::vector<EdgePtr> sorted;
        sorted.reserve(gs.edges.size());
        for (const auto& e : gs.edges) {
            sorted.push_back(&e);
        }

        std::sort(sorted.begin(), sorted.end(),
                  [](EdgePtr a, EdgePtr b) {
                      return a->weight > b->weight;
                  });

        size_t keep = static_cast<size_t>(
            std::floor(globalFraction * sorted.size())
        );
        if (keep == 0 && !sorted.empty() && globalFraction > 0.0) {
            keep = 1; // ensure at least one if fraction > 0
        }

        for (size_t i = 0; i < keep; ++i) {
            const Edge* e = sorted[i];
            candidateKeys.insert(EdgeKey(e->u, e->v));
        }
    }

    // -------------------------------------------------
    //  Materialise candidate edges
    //  (iterate original edges so ordering is deterministic)
    // -------------------------------------------------
    result.reserve(candidateKeys.size());
    for (const auto& e : gs.edges) {
        EdgeKey key(e.u, e.v);
        if (candidateKeys.find(key) != candidateKeys.end()) {
            result.push_back(e);
        }
    }

    // -------------------------------------------------
    //  Randomize ordering inline - Using seed to make results reproducible
    // -------------------------------------------------
    std::random_device rd;
    // use rd instead of int seed for real 'random' results
    std::mt19937 gen(11821283);
    std::shuffle(result.begin(), result.end(), gen);

    return result;
}
