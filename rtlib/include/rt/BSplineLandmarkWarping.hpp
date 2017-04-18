#pragma once

#include <itkLandmarkDisplacementFieldSource.h>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
class BSplineLandmarkWarping : public LandmarkRegistrationBase
{
public:
    using TransformInitializer =
        itk::LandmarkDisplacementFieldSource<DeformationField>;
    using Transform = TransformInitializer::KernelTransformType;

    Transform::Pointer compute();

    Transform::Pointer getTransform() { return output_; }

    void setFixedImage(const Image8UC3::Pointer f) { fixedImg_ = f; }

private:
    Transform::Pointer output_;
    Image8UC3::Pointer fixedImg_;
};
}