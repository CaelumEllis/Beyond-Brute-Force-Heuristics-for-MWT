//
// Created by Caelum Ellis on 16/11/2025.
//
#include "CandidateEdgeFilter.h"
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <random>

using EdgePtr = const Edge*;

std::vector<Edge> CandidateEdgeFilter::buildCandidateSet(const GraphState& gs) {
    return buildCandidateSet(gs, defaultEdgeSelectionPolicy);
}

void CandidateEdgeFilter::updateCandidatesAfterFlip(
    std::vector<Edge>& candidates,
    const GraphState& gs,
    const FlipResult& flip,
    bool aggressiveUpdate) {
    updateCandidatesAfterFlip(candidates, gs, flip, defaultEdgeSelectionPolicy, aggressiveUpdate);
}

bool CandidateEdgeFilter::isGoodCandidate(
    const Edge& e,
    const GraphState& gs,
    const CandidatePolicy& policy) {
    // Take Global top fraction
    // Compare length rank against threshold
    static thread_local std::vector<double> allLengths;
    allLengths.clear();
    allLengths.reserve(gs.edges.size());

    for (const auto& x : gs.edges) {
        allLengths.push_back(x.weight);
    }

    std::sort(allLengths.begin(), allLengths.end(), std::greater<>());

    // always pick at least one edge
    size_t cutoffIndex = std::max<size_t>(1,static_cast<size_t>(std::floor(policy.globalFraction * allLengths.size())));

    double cutoffLength = allLengths[cutoffIndex - 1];

    if (e.weight >= cutoffLength) return true;


    // select top k per vertex
    const int u = e.u, v = e.v;

    auto checkTopK = [&](int vertex) {
        std::vector<double> incident;
        incident.reserve(gs.adjacency.at(vertex).size());

        for (int nb : gs.adjacency.at(vertex)) {
            const Edge* ep = gs.getEdge(vertex, nb);
            if (ep) incident.push_back(ep->weight);
        }

        if (incident.empty()) return false;

        std::sort(incident.begin(), incident.end(), std::greater<>());
        int k = std::min(policy.perVertex, (int)incident.size());
        return e.weight >= incident[k - 1];
    };

    return checkTopK(u) || checkTopK(v);
}

std::vector<Edge> CandidateEdgeFilter::buildCandidateSet(
    const GraphState& gs,
    const CandidatePolicy& policy) {
    std::vector<Edge> result;
    if (gs.edges.empty()) return result;

    // generate full/global candidate set based on policy
    for (const auto& e : gs.edges) {
        if (isGoodCandidate(e, gs, policy))
            result.push_back(e);
    }

    // shuffle to avoid deterministic ordering
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(result.begin(), result.end(), rng);

    return result;
}



void CandidateEdgeFilter::updateCandidatesAfterFlip(
    std::vector<Edge>& candidates,
    const GraphState& gs,
    const FlipResult& flip,
    const CandidatePolicy& policy,
    bool aggressiveUpdate) {

    if (!flip.legal) return;

    // Full rebuild if aggressive flag is set
    if (aggressiveUpdate) {
        candidates = buildCandidateSet(gs, policy);
        return;
    }

    EdgeKey oldDiag(flip.b, flip.d);
    EdgeKey newDiag(flip.a, flip.c);

    // Remove old edge if present
    candidates.erase(
        std::remove_if(
            candidates.begin(), candidates.end(),
            [&](const Edge& e){ return EdgeKey(e.u,e.v)==oldDiag; }
        ),
        candidates.end()
    );

    // Insert new diagonal if qualifies
    if (const Edge* e = gs.getEdge(newDiag.u,newDiag.v);
        e && isGoodCandidate(*e, gs, policy))
    {
        candidates.push_back(*e);
    }

    // Local adjustments: only update edges touching (a,b,c,d)
    std::unordered_set<EdgeKey, EdgeKeyHash> seen;

    for (int v : { flip.a, flip.b, flip.c, flip.d }) {
        for (int nb : gs.adjacency.at(v)) {

            EdgeKey key(v, nb);
            if (seen.count(key)) continue;
            seen.insert(key);

            const Edge* e = gs.getEdge(key.u, key.v);
            if (!e) continue;

            bool good = isGoodCandidate(*e, gs, policy);

            bool existsInSet =
                std::any_of(candidates.begin(), candidates.end(),
                            [&](const Edge& ex){ return EdgeKey(ex.u,ex.v)==key; });

            if (good && !existsInSet)
                candidates.push_back(*e);

            else if (!good && existsInSet)
                candidates.erase(
                    std::remove_if(
                        candidates.begin(), candidates.end(),
                        [&](const Edge &ex){ return EdgeKey(ex.u,ex.v)==key; }
                    ),
                    candidates.end()
                );
        }
    }

    // Shuffle again to break selection bias
    static std::mt19937 rng(std::random_device{}());
    std::shuffle(candidates.begin(), candidates.end(), rng);
}
