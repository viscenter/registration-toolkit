#include "rt/LandmarkIO.hpp"

using namespace rt;
namespace fs = boost::filesystem;

void LandmarkIO::read()
{
    // Check that input is set
    if (!fs::exists(landmarksPath_) || fixedImage_ == nullptr ||
        movingImage_ == nullptr) {
        throw std::runtime_error("Invalid input. Unable to read.");
    }

    // Reset landmark containers
    fixedLandmarks_.clear();
    movingLandmarks_.clear();

    // Read the landmarks file
    LandmarkRegistration::Landmark fixedPoint, movingPoint;
    Image8UC3::IndexType fixedIndex, movingIndex;
    size_t fixedX, fixedY, movingX, movingY;

    std::ifstream ifs(landmarksPath_.string());
    while (ifs.good()) {
        ifs >> fixedX >> fixedY >> movingX >> movingY;

        fixedIndex[0] = fixedX;
        fixedIndex[1] = fixedY;
        movingIndex[0] = movingX;
        movingIndex[1] = movingY;

        // Transform landmarks in case spacing still gets used
        fixedImage_->TransformIndexToPhysicalPoint(fixedIndex, fixedPoint);
        movingImage_->TransformIndexToPhysicalPoint(movingIndex, movingPoint);

        fixedLandmarks_.push_back(fixedPoint);
        movingLandmarks_.push_back(movingPoint);
    }
    ifs.close();
}