#pragma once

#include <itkBSplineTransform.h>
#include <itkLandmarkDisplacementFieldSource.h>

#include "rt/ImageTypes.hpp"
#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @class BSplineLandmarkWarping
 * @brief Generate a B-spline-based transformation that maps an ordered set of
 * landmarks onto a fixed set of landmarks
 * @ingroup landmark
 */
class BSplineLandmarkWarping : public LandmarkRegistrationBase
{
public:
    /** @brief Transform type returned by this class */
    using Transform = itk::BSplineTransform<double, 2>;

    /**
     * @brief Set the fixed image
     *
     * This must be set in order to compute the transform.
     */
    void setFixedImage(const Image8UC3::Pointer& f) { fixedImg_ = f; }

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