#pragma once

/** @file */

#include <itkAffineTransform.h>

#include "rt/LandmarkRegistrationBase.hpp"

namespace rt
{
/**
 * @brief Generate an affine transformation that maps an ordered set of
 * landmarks onto a fixed set of landmarks
 */
class AffineLandmarkRegistration : public LandmarkRegistrationBase
{
public:
    /** @brief Transform type returned by this class */
    using Transform = itk::AffineTransform<double, 2>;

    /** @brief Report error metrics to the console while processing */
    void setReportMetrics(bool i);

    /** @copydoc setReportMetrics(bool) */
    [[nodiscard]] auto getReportMetrics() -> bool;

    /** @brief Compute the transform */
    auto compute() -> Transform::Pointer;

    /** @brief Return the computed transform */
    auto getTransform() -> Transform::Pointer;

private:
    /** Transform */
    Transform::Pointer output_;
    /** Report error metrics during processing */
    bool reportMetrics_{false};
};
}  // namespace rt