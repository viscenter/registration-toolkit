#pragma once

/** @file */

#include <itkBSplineTransform.h>
#include <opencv2/core.hpp>

namespace rt
{

/**
 * @class DeformableRegistration
 * @brief B-Spline-based deformable registration
 *
 * Computes a B-Spline transform that aligns the fine details of two input
 * images. Assumes the input images have already been roughly aligned using
 * some sort of landmark registration algorithm.
 *
 * This class is a simplified wrapper around ITK's ImageRegistrationMethod. If
 * you want fine-grained control, you should probably use that instead.
 *
 */
class DeformableRegistration
{
public:
    /** Default max number of optimization iterations  */
    static constexpr size_t DEFAULT_ITERATIONS = 100;
    /** Default relaxation factor */
    static constexpr double DEFAULT_RELAXATION = 0.85;
    /** Default gradient magnitude tolerance */
    static constexpr double DEFAULT_GRAD_MAG_TOLERANCE = 0.0001;
    /** BSpline transform type */
    using Transform = itk::BSplineTransform<double, 2, 3>;

    /**@{*/
    /** @brief Set the fixed (target) image for registration */
    void setFixedImage(const cv::Mat& i);
    /** @brief Set the moving (transformed) image for registration */
    void setMovingImage(const cv::Mat& i);
    /** @brief Set optimizer iteration limit
     *
     * Optimizer stops after this many iterations.
     */
    void setNumberOfIterations(size_t i);
    /**@}*/

    /**@{*/
    /** @brief Run registration and return the computed transform */
    auto compute() -> Transform::Pointer;
    /**@}*/

    /**@{*/
    /** @brief Return the computed transform */
    auto getTransform() -> Transform::Pointer;
    /**@}*/

private:
    /** Fixed input image */
    cv::Mat fixedImage_;
    /** Moving input image */
    cv::Mat movingImage_;

    /** Output BSpline transform */
    Transform::Pointer output_;

    /** Optimizer iteration limit */
    size_t iterations_{DEFAULT_ITERATIONS};
    /** Optimizer step length is reduced by this factor each iteration */
    double relaxationFactor_{DEFAULT_RELAXATION};
    /** Stop condition if change in metric is less than this value */
    double gradientMagnitudeTolerance_{DEFAULT_GRAD_MAG_TOLERANCE};
};
}  // namespace rt
