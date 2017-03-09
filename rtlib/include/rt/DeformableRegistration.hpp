#pragma once

#include <itkBSplineTransform.h>

#include "rt/ImageTypes.hpp"

namespace rt
{

/** @class DeformableRegistration
 * @brief BSpline-based deformable registration
 *
 * Computes a BSpline transform that aligns the fine details of two input
 * images. Assumes the input images have already been roughly aligned using
 * LandmarkRegistration.
 *
 * This class is a simplified wrapper around ITK's ImageRegistrationMethod. If
 * you want fine-grained control, you should probably use that instead.
 *
 * @ingroup rtlib
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
    /** Default constructor */
    DeformableRegistration()
        : iterations_(DEFAULT_ITERATIONS)
        , relaxationFactor_(DEFAULT_RELAXATION)
        , gradientMagnitudeTolerance_(DEFAULT_GRAD_MAG_TOLERANCE)
    {
    }
    /**@}*/

    /**@{*/
    /** @brief Set the fixed (target) image for registration */
    void setFixedImage(Image8UC3::Pointer i) { fixedImage_ = i; }
    /** @brief Set the moving (transformed) image for registration */
    void setMovingImage(Image8UC3::Pointer i) { movingImage_ = i; }
    /** @brief Set optimizer iteration limit
     *
     * Optimizer stops after this many iterations.
     */
    void setNumberOfIterations(size_t i) { iterations_ = i; }
    /**@}*/

    /**@{*/
    /** @brief Run registration and return the computed transform */
    Transform::Pointer compute();
    /**@}*/

    /**@{*/
    /** @brief Return the computed transform */
    Transform::Pointer getTransform() { return output_; }
    /** @brief Interpolate the moving image through the computed transform */
    Image8UC3::Pointer getTransformedImage();
    /**@}*/

private:
    /** Fixed input image */
    Image8UC3::Pointer fixedImage_;
    /** Moving input image */
    Image8UC3::Pointer movingImage_;

    /** Output BSpline transform */
    Transform::Pointer output_;

    /** Optimizer iteration limit */
    size_t iterations_;
    /** Optimizer step length is reduced by this factor each iteration */
    double relaxationFactor_;
    /** Stop condition if change in metric is less than this value */
    double gradientMagnitudeTolerance_;
};
}
