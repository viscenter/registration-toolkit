#pragma once

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
class AffineLandmarkRegistration : public LandmarkRegistrationBase
{
public:
    using Transform = itk::AffineTransform<double, 2>;
    using TransformInitializer =
        itk::LandmarkBasedTransformInitializer<Transform, Image8UC3, Image8UC3>;

    Transform::Pointer compute();

    Transform::Pointer getTransform() { return output_; }
    
private:
    Transform::Pointer output_;
};
}