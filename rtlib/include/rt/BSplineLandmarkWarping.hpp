#pragma once

#include <itkLandmarkDisplacementFieldSource.h>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class BSplineLandmarkWarping
 * @brief Generate a B-spline based displacement field transformation using
 * source and target landmarks
 * @ingroup landmark
 */
class BSplineLandmarkWarping : public LandmarkRegistrationBase
{
public:
    /** Displacement field generator type */
    using TransformInitializer =
        itk::LandmarkDisplacementFieldSource<DeformationField>;

    /** @brief Transform type returned by this class */
    using Transform = TransformInitializer::KernelTransformType;

    /** @brief Set the fixed image
     *
     * This must be set in order to compute the transform.
     */
    void setFixedImage(const Image8UC3::Pointer f) { fixedImg_ = f; }

    /** @brief Compute the transform */
    Transform::Pointer compute();

    /** @brief Return the computed transform */
    Transform::Pointer getTransform();

private:
    /** Transform */
    Transform::Pointer output_;

    /** Fixed image */
    Image8UC3::Pointer fixedImg_;
};
}