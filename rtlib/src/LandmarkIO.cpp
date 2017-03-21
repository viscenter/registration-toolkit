#include "rt/LandmarkIO.hpp"

using namespace rt;

void LandmarkIO::read() {
    fixedLandmarks_ = LandmarkRegistration::LandmarkTransformInitializer::
        New()::LandmarkPointContainer;
    movingLandmarks_ = LandmarkRegistration::LandmarkTransformInitializer::
        New()::LandmarkPointContainer;

    if (!fs::exists(landmarksPath_) || fixedImage_ == nullptr ||
        movingImage_ == nullptr) {
        throw std::runtime_error("Missing input image");
    }
    Landmark fixedPoint, movingPoint;
    Image8UC3::IndexType fixedIndex, movingIndex;
    size_t fixedX, fixedY, movingX, movingY;

    std::ifstream landmarksFile(landmarksPath_.string());
    while (!landmarksFile.fail()) {
        landmarksFile >> fixedX >> fixedY >> movingX >> movingY;

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
    landmarksFile.close();
}