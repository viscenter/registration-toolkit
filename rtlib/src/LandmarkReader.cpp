#include "rt/LandmarkReader.hpp"

using namespace rt;
namespace fs = boost::filesystem;

void LandmarkReader::read()
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
    rt::Landmark fixedPoint, movingPoint;
    itk::ContinuousIndex<double, 2> fixedIndex, movingIndex;
    double fixedX, fixedY, movingX, movingY;

    std::ifstream ifs(landmarksPath_.string());
    while (ifs.good()) {
        ifs >> fixedX >> fixedY >> movingX >> movingY;

        fixedIndex[0] = fixedX;
        fixedIndex[1] = fixedY;
        movingIndex[0] = movingX;
        movingIndex[1] = movingY;

        // Transform landmarks in case spacing still gets used
        fixedImage_->TransformContinuousIndexToPhysicalPoint(
            fixedIndex, fixedPoint);
        movingImage_->TransformContinuousIndexToPhysicalPoint(
            movingIndex, movingPoint);

        fixedLandmarks_.push_back(fixedPoint);
        movingLandmarks_.push_back(movingPoint);
    }
    ifs.close();
}