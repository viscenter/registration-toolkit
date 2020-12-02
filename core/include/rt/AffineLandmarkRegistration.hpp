#pragma once

/** @file */

#include <itkAffineTransform.h>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class AffineLandmarkRegistration
 * @brief Generate an affine transformation that maps an ordered set of
 * landmarks onto a fixed set of landmarks
 */
class AffineLandmarkRegistration : public LandmarkRegistrationBase
{
public:
    /** @brief Transform type returned by this class */
    using Transform = itk::AffineTransform<double, 2>;

    /** @brief Compute the transform */
    Transform::Pointer compute();

    /** @brief Return the computed transform */
    Transform::Pointer getTransform();

private:
    /** Transform */
    Transform::Pointer output_;
};
}  // namespace rt