//
// Created by Caelum Ellis on 16/11/2025.
//
#ifndef CANDIDATEEDGEFILTER_H
#define CANDIDATEEDGEFILTER_H

#include <vector>
#include "GraphState.h"
#include "FlipCriteria.h"

class CandidateEdgeFilter {

public:

    struct CandidatePolicy {
        int    perVertex;
        double globalFraction;
    };

    // Full explicit build
    static std::vector<Edge> buildCandidateSet(
        const GraphState& gs,
        const CandidatePolicy& policy
    );

    // Explicit update after flip
    static void updateCandidatesAfterFlip(
        std::vector<Edge>& candidates,
        const GraphState& gs,
        const FlipResult& flip,
        const CandidatePolicy& policy,
        bool aggressiveUpdate = false
    );

    // Convenience overloads (declared only, defined in .cpp)
    static std::vector<Edge> buildCandidateSet(const GraphState& gs);
    static void updateCandidatesAfterFlip(
        std::vector<Edge>& candidates,
        const GraphState& gs,
        const FlipResult& flip,
        bool aggressiveUpdate = false
    );

private:

    static bool isGoodCandidate(
        const Edge& e,
        const GraphState& gs,
        const CandidatePolicy& policy
    );

    inline static CandidatePolicy defaultPolicy{4, 0.50};


};

#endif
