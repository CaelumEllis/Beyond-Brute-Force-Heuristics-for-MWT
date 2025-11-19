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

void SimulatedAnnealing::configureDynamic(const GraphState& gs, const std::vector<Edge>& candidates) {

    // Estimate average |delta E| from sample flips
    int sampleCount = std::min<int>(800, candidates.size());

    double sum = 0;
    int valid = 0;

    for (int i = 0; i < sampleCount; i++) {
        auto &e = candidates[i];
        auto flip = FlipCriteria::isFlipLegal(gs, e.u, e.v);
        if (!flip.legal) continue;

        double delta = std::abs(computeWeightChange(gs, flip));
        if (delta > 0) {
            sum += delta;
            valid++;
        }
    }

    double Eavg = (valid > 0 ? sum / valid : 0.1);

    // auto adjusted parameters based on average |ΔE|
    initialTemperature = 2.5 * Eavg;
    minTemperature     = Eavg / 1000.0;
    // scales with point set
    maxIterations      = gs.edges.size() * 300;

    adaptiveCooling = true;
}


double SimulatedAnnealing::computeWeightChange(const GraphState& gs, const FlipResult& flip) {

    const Edge* oldEdge = gs.getEdge(flip.b, flip.d);
    if (!oldEdge) return 0.0;

    double oldLen = oldEdge->weight;

    // compute new diagonal explicitly
    const auto& pA = gs.points[flip.a];
    const auto& pC = gs.points[flip.c];

    double dx = pA.first - pC.first;
    double dy = pA.second - pC.second;
    double newLen = std::sqrt(dx * dx + dy * dy);

    return std::log(newLen / oldLen);
}


void SimulatedAnnealing::run(GraphState& gs)
{
    auto candidates = CandidateEdgeFilter::buildCandidateSet(gs);
    std::mt19937 rng(std::random_device{}());

    // reset for this run
    totalAccepted = 0;
    double T = initialTemperature;

    for (int i = 0; i < maxIterations && T > minTemperature; i++) {

        if (candidates.size() < 0.5 * gs.edges.size())
            candidates = CandidateEdgeFilter::buildCandidateSet(gs);

        const Edge& e = candidates[rng() % candidates.size()];
        auto flip = FlipCriteria::isFlipLegal(gs, e.u, e.v);
        if (!flip.legal) continue;

        double delta = computeWeightChange(gs, flip);
        bool accept = (delta < 0) || (exp(-delta / T) > ((double)rng() / rng.max()));

        if (accept) {
            totalAccepted++;
            gs.FlipEdge(flip);
            CandidateEdgeFilter::updateCandidatesAfterFlip(candidates, gs, flip);
        }

        // dynamic cooling option
        if (adaptiveCooling)
            T *= (delta < 0 ? 0.99995 : 0.9993);
        else
            T *= coolingRate;
    }
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
            // if the proposed flip is better, accept it
            if (delta < 0) {
                gs.FlipEdge(flip);

                // Update candidate list incrementally after the flip
                CandidateEdgeFilter::updateCandidatesAfterFlip(candidates, gs, flip);

                improved = true;
                // restart search to always apply best-first improvements
                break;
            }
        }
    }
}

