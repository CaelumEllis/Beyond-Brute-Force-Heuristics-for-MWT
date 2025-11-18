//
// Created by Caelum Ellis on 16/11/2025.
//
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "GraphState.h"

class SimulatedAnnealing {
public:
    double initialTemperature = 1.0;
    double minTemperature = 1e-6;
    double coolingRate = 0.9995;
    int maxIterations = 200000;

    bool adaptiveCooling = true;
    int totalAccepted = 0;

    void configureDynamic(const GraphState& gs, const std::vector<Edge>& candidates);
    void run(GraphState& gs);
    static double computeWeight(const GraphState& gs);
    static void greedyImprove(GraphState& gs);

private:
    static double computeWeightChange(const GraphState& gs, const FlipResult& flip);
};


#endif

