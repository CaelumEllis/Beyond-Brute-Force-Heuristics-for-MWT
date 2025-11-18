//
// Created by Caelum Ellis on 16/11/2025.
//
#include "SimulatedAnnealing.h"
#include "CandidateEdgeFilter.h"
#include "FlipCriteria.h"
#include <cmath>
#include <random>
#include <iostream>

double SimulatedAnnealing::computeWeight(const GraphState& gs) {
    double sum = 0;
    for (const auto& e : gs.edges)
        sum += e.weight;  // simple smoothness metric
    return sum;
}

// static double computeWeightChange(const GraphState& gs, const FlipResult& flip) {
//     const Edge* oldEdge = gs.getEdge(flip.b, flip.d);
//     double oldLen2 = oldEdge ? oldEdge->weight * oldEdge->weight : 0;
//
//     const Edge* newEdge = gs.getEdge(flip.a, flip.c);
//     double newLen2 = newEdge ? newEdge->weight * newEdge->weight : 0;
//
//     return newLen2 - oldLen2;
// }

static double computeWeightChange(const GraphState& gs, const FlipResult& flip) {
    // old diagonal exists in the current triangulation
    const Edge* oldEdge = gs.getEdge(flip.b, flip.d);
    if (!oldEdge) return 0.0;

    double oldLen = oldEdge->weight;

    // compute new diagonal length (a,c) from coordinates (not from existing edges)
    const auto& pA = gs.points[flip.a];
    const auto& pC = gs.points[flip.c];

    double dx = pA.first  - pC.first;
    double dy = pA.second - pC.second;
    double newLen = std::sqrt(dx*dx + dy*dy);

    // log-ratio metric (good for SA scaling)
    return std::log(newLen / oldLen);
}

void SimulatedAnnealing::run(GraphState& gs) const {

    auto candidates = CandidateEdgeFilter::buildCandidateSet(gs);
    size_t candidateCount = candidates.size();
    if (candidates.empty()) {
        std::cerr << "No candidate edges available.\n";
        return;
    }

    std::mt19937 rng(std::random_device{}());

    double T = initialTemperature;
    int iterations = 0;

    while (T > minTemperature && iterations < maxIterations) {

        if (candidates.size() <= candidateCount / 3) {
            candidates = CandidateEdgeFilter::buildCandidateSet(gs);
        }

        const Edge& e = candidates[rng() % candidates.size()];
        auto flip = FlipCriteria::isFlipLegal(gs, e.u, e.v);

        if (flip.legal) {
            double delta = computeWeightChange(gs, flip);

            bool accept = (delta < 0) ||(exp(-delta / T) > (double)rng() / rng.max());

            if (accept) {
                gs.FlipEdge(flip);

                CandidateEdgeFilter::updateCandidatesAfterFlip(
                    candidates, gs, flip
                );
            }
        }

        T *= coolingRate;
        iterations++;
    }

    std::cout << "Simulated Annealing Completed.\n";
}

// just used to prove that SA is working properly
void SimulatedAnnealing::greedyImprove(GraphState& gs) {

    bool improved = true;

    // Initial candidate list
    auto candidates = CandidateEdgeFilter::buildCandidateSet(gs);

    while (improved) {
        improved = false;

        // If too many candidates became invalid, rebuild from scratch
        if (candidates.size() < 0.3 * gs.edges.size()) {
            candidates = CandidateEdgeFilter::buildCandidateSet(gs);
        }

        for (const auto& e : candidates) {
            auto flip = FlipCriteria::isFlipLegal(gs, e.u, e.v);
            if (!flip.legal) continue;

            double delta = computeWeightChange(gs, flip);

            if (delta < 0) { // improvement!
                gs.FlipEdge(flip);

                // Update candidate list incrementally after the flip
                CandidateEdgeFilter::updateCandidatesAfterFlip(candidates, gs, flip);

                improved = true;
                break;  // restart search to always apply best-first improvements
            }
        }
    }
}

