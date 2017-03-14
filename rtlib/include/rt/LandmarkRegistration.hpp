#pragma once

#include "ImageTypes.hpp"

using LandmarkContainer = LandmarkTransformInitializer::LandmarkPointContainer;

namespace rt
{
class LandmarkRegistration {
public:
    LandmarkRegistration(
        const Image8UC3& fixedImage, 
        const Image8UC3& movingImage,
        const LandmarkContainer& fixedLandmarks, LandmarkContainer& movingLandmarks)
        : fixedImage_(fixedImage), movingImage_(movingImage), fixedLandmarks_(fixedLandmarks),
        movingLandmarks_(movingLandmarks)
    {
    }

    AffineTransform::Pointer getTransform();
    void setFixedImage(const Image8UC3& fixedImage) { fixedImage_ = fixedImage; }
    void setMovingImage(const Image8UC3& movingImage) { movingImage_ = movingImage; }
    void setFixedLandmarkContainer(const LandmarkContainer& fixedLandmarks) { fixedLandmarks_ = fixedLandmarks; }
    void setMovingLandmarkContainer(const LandmarkContainer& fixedLandmarks) { fixedLandmarks_ = fixedLandmarks; }

private:
    /** Gets the Affine Transform from the given fixed and moving landmarks */ 
    void compute_();
    void clear_();
    void generate_affine_transform_();

    AffineTransform::Pointer landmarkTransform_;
    Image8UC3 fixedImage_;
    Image8UC3 movingImage_;
    LandmarkContainer fixedLandmarks_;
    LandmarkContainer movingLandmarks_;
}
}