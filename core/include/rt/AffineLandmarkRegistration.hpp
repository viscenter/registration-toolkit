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

    static constexpr bool DEFAULT_OUTPUT_METRIC = false;

    /** @brief Set the bool for outputing the metric */
    void setOutputMetric(bool i);
    /** @brief get the bool for outputting the metric */
    bool getOutputMetric();

    /** @brief Compute the transform */
    auto compute() -> Transform::Pointer;

    /** @brief Return the computed transform */
    auto getTransform() -> Transform::Pointer;

private:
    /** Transform */
    Transform::Pointer output_;
    /** outputs the metric value */
    bool outputMetric_{DEFAULT_OUTPUT_METRIC};
};
}  // namespace rt