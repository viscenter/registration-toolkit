#pragma once

#include "ImageTypes.hpp"

namespace rt
{
class LandmarkRegistration {
public:
    using Transform = itk::AffineTransform<double, 2>;
    using LandmarkTransformInitializer =
        itk::LandmarkBasedTransformInitializer<Transform, Image8UC3, Image8UC3>;
    using LandmarkContainer =
        LandmarkTransformInitializer::LandmarkPointContainer;

    LandmarkRegistration() {}

    Transform::Pointer getTransform();
    void setFixedImage(const Image8UC3::Pointer fixedImage)
    {
        fixedImage_ = fixedImage;
    }
    void setMovingImage(const Image8UC3::Pointer movingImage)
    {
        movingImage_ = movingImage;
    }
    void setFixedLandmarkContainer(const LandmarkContainer& fixedLandmarks) { fixedLandmarks_ = fixedLandmarks; }
    void setMovingLandmarkContainer(const LandmarkContainer& fixedLandmarks) { fixedLandmarks_ = fixedLandmarks; }

private:
    /** Gets the Affine Transform from the given fixed and moving landmarks */ 
    void compute_();
    void clear_();
    void generate_affine_transform_();

    Transform::Pointer landmarkTransform_;
    Image8UC3::Pointer fixedImage_;
    Image8UC3::Pointer movingImage_;
    LandmarkContainer fixedLandmarks_;
    LandmarkContainer movingLandmarks_;
};
}