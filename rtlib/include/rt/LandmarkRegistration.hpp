#pragma once

#include "ImageTypes.hpp"

namespace rt
{
class LandmarkRegistration
{
public:
    using Transform = itk::AffineTransform<double, 2>;
    using TransformInitializer =
        itk::LandmarkBasedTransformInitializer<Transform, Image8UC3, Image8UC3>;
    using LandmarkContainer = TransformInitializer::LandmarkPointContainer;
    using Landmark = TransformInitializer::LandmarkPointType;

    Transform::Pointer compute();

    Transform::Pointer getTransform() { return output_; }
    void setFixedLandmarks(const LandmarkContainer& l) { fixedLdmks_ = l; }
    void setMovingLandmarks(const LandmarkContainer& l) { movingLdmks_ = l; }

private:
    Transform::Pointer output_;
    LandmarkContainer fixedLdmks_;
    LandmarkContainer movingLdmks_;
};
}