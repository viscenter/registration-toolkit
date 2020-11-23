#include "rt/io/LandmarkWriter.hpp"

#include <fstream>
#include <iostream>

using namespace rt;
namespace fs = rt::filesystem;

void LandmarkWriter::setPath(const fs::path& p) { path_ = p; }

void LandmarkWriter::setFixedLandmarks(const LandmarkContainer& f)
{
    fixed_ = f;
}

void LandmarkWriter::setMovingLandmarks(const LandmarkContainer& m)

{
    moving_ = m;
}

void LandmarkWriter::write()
{
    // Check parameters
    if (path_.empty()) {
        throw std::runtime_error("Empty file path");
    }

    if (fixed_.size() != moving_.size()) {
        throw std::runtime_error("Landmark containers of different size");
    }

    // Open file
    std::ofstream file(path_.string());
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open output file");
    }

    // Write the landmarks
    for (size_t i = 0; i < fixed_.size(); i++) {
        file << fixed_.at(i)[0] << " ";
        file << fixed_.at(i)[1] << " ";
        file << moving_.at(i)[0] << " ";
        file << moving_.at(i)[1] << "\n";
    }

    // Close file
    file.close();
}