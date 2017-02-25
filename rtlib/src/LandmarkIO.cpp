#include "LandmarkIO.hpp"

void LandmarkIO::read() {
    clear_(); 
    read_landmark_files_();
}

void LandmarkIO::read_landmarks_file_() {
    Landmark fixedPoint, movingPoint;
    Image::IndexType fixedIndex, movingIndex;
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

void LandmarkIO::clear_() {
    /** TODO: Figure out clearing mechanism for ITK::LandmarkPointContainer */
    return;
}