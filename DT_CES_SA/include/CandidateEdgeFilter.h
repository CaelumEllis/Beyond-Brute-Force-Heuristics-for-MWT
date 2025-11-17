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

    // Global single source of truth
    inline static CandidatePolicy defaultPolicy{2, 1.0/10.0};

    // Explicit build with a supplied policy
    static std::vector<Edge> buildCandidateSet(
        const GraphState& gs,
        const CandidatePolicy& policy
    );

    // Convenience wrapper: always uses globally defined policy
    static std::vector<Edge> buildCandidateSet(const GraphState& gs) {
        return buildCandidateSet(gs, defaultPolicy);
    }

    // Explicit update with a supplied policy
    static void updateCandidatesAfterFlip(
        std::vector<Edge>& candidates,
        const GraphState& gs,
        const FlipResult& flip,
        const CandidatePolicy& policy,
        bool aggressiveUpdate = false
    );

    // Convenience wrapper: always uses global policy
    static void updateCandidatesAfterFlip(
        std::vector<Edge>& candidates,
        const GraphState& gs,
        const FlipResult& flip,
        bool aggressiveUpdate = false
    ) {
        updateCandidatesAfterFlip(candidates, gs, flip, defaultPolicy, aggressiveUpdate);
    }

private:
    static bool isGoodCandidate(
        const Edge& e,
        const GraphState& gs,
        const CandidatePolicy& policy
    );
};


#endif // CANDIDATEEDGEFILTER_H
