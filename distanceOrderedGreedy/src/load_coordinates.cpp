//
// Created by Caelum Ellis on 16/11/2025.
//
#include "load_coordinates.h"
#include <fstream>
#include <iostream>

std::vector<Coordinate> loadCoordinatesFromFile(const std::string& filename) {
    std::ifstream file(filename);

    if (!file) {
        throw std::runtime_error("Error: Could not open the file: " + filename);
    }

    int count = 0;
    file >> count;

    if(count <= 0) {
        throw std::runtime_error("Error: Invalid or missing coordinate count in file.");
    }

    std::vector<Coordinate> coordinates;
    coordinates.reserve(count);

    for (int i = 0; i < count; i++) {
        double x, y;
        if (!(file >> x >> y)) {
            throw std::runtime_error("Error: File format incorrect or missing data.");
        }
        coordinates.push_back({x, y});
    }

    return coordinates;
}