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
    /** Default mesh fill size */
    static constexpr uint32_t DEFAULT_MESH_FILL_SIZE = 12;
    /** BSpline transform type */
    using Transform = itk::BSplineTransform<double, 2, 3>;

    /**@{*/
    /** @brief Set the fixed (target) image for registration */
    void setFixedImage(const cv::Mat& i);
    /** @brief Set the moving (transformed) image for registration */
    void setMovingImage(const cv::Mat& i);
    /**
     * @brief Set optimizer iteration limit
     *
     * Optimizer stops after this many iterations.
     */
    void setNumberOfIterations(size_t i);
    /** @brief Set the Mesh Fill Size */
    void setMeshFillSize(uint32_t i);
    /** @brief Set the Gradient Magnitude Tolerance */
    void setGradientMagnitudeTolerance(double i);
    /** @brief Set the bool for outputing the metric */
    void setOutputMetric(bool i);
    /**@}*/

    /**@}*/
    /** @brief Get the Mesh Fill Size */
    [[nodiscard]] auto getMeshFillSize() const -> uint32_t;
    /** @brief Get the Gradient Magnitude Tolerance */
    [[nodiscard]] auto getGradientMagnitudeTolerance() const -> double;
    /** @brief get the bool for outputting the metric */
    [[nodiscard]] auto getOutputMetric() const -> bool;
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
    /** Mesh fill size */
    uint32_t meshFillSize_{DEFAULT_MESH_FILL_SIZE};
    /** Optimizer step length is reduced by this factor each iteration */
    double relaxationFactor_{DEFAULT_RELAXATION};
    /** Stop condition if change in metric is less than this value */
    double gradientMagnitudeTolerance_{DEFAULT_GRAD_MAG_TOLERANCE};
    /** outputs the metric values */
    bool outputMetric_{false};
};
}  // namespace rt
