//
// Created by Caelum Ellis on 16/11/2025.
//
#ifndef SIMULATED_ANNEALING_H
#define SIMULATED_ANNEALING_H

#include "GraphState.h"

class SimulatedAnnealing {
public:
    double initialTemperature = 1.0;
    double minTemperature = 0.001;
    double coolingRate = 0.995;
    int maxIterations = 5000;


    void run(GraphState& gs) const;
    static double computeWeight(const GraphState& gs);
    static void greedyImprove(GraphState& gs);
};

#endif

