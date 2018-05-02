#include "rt/io/LandmarkWriter.hpp"

#include <iostream>

using namespace rt;

void LandmarkWriter::write()
{
    // Check parameters
    if (outputPath_.empty()) {
        throw std::runtime_error("Empty file path");
    }

    if (fixedLandmarks_.empty() || movingLandmarks_.empty()) {
        throw std::runtime_error("Empty landmark containers");
    }

    if (fixedLandmarks_.size() != movingLandmarks_.size()) {
        throw std::runtime_error("Landmark containers of different size");
    }

    // Open file
    std::ofstream file(outputPath_.string());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open output file");
    }

    // Write the landmarks
    for (size_t i = 0; i < fixedLandmarks_.size(); i++) {
        file << fixedLandmarks_.at(i)[0] << " ";
        file << fixedLandmarks_.at(i)[1] << " ";
        file << movingLandmarks_.at(i)[0] << " ";
        file << movingLandmarks_.at(i)[1] << "\n";
    }

    // Close file
    file.close();
}