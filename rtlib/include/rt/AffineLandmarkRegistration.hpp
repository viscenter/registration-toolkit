#pragma once

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class AffineLandmarkRegistration
 * @brief Generate an affine transformation that best maps a set of points
 * onto a second set of points
 * @ingroup landmark
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
}