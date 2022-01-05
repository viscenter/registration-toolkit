#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/DeformableRegistration.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt::graph
{

/**
 * @brief B-Spline Deformable Registration
 * @see DeformableRegistration
 */
class DeformableRegistrationNode : public smgl::Node
{
public:
    /** Default constructor */
    DeformableRegistrationNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed image */
    smgl::InputPort<cv::Mat> fixedImage;
    /** @brief Moving image */
    smgl::InputPort<cv::Mat> movingImage;
    /** @brief Mesh Fill Size */
    smgl::InputPort<unsigned> meshFillSize;
    /** @brief Gradient magnitude tolerance */
    smgl::InputPort<double> gradientTolerance;
    /** @brief Deformable iterations */
    smgl::InputPort<int> iterations;
    /** @copydoc DeformableRegistration::setReportMetrics(bool) */
    smgl::InputPort<bool> reportMetrics;
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Final transform port */
    smgl::OutputPort<Transform::Pointer> transform;
    /**@}*/

private:
    /** Registration method */
    DeformableRegistration reg_;
    /** Iterations */
    int iters_{DeformableRegistration::DEFAULT_ITERATIONS};
    /** Final transform */
    Transform::Pointer tfm_;
    /** Graph serialize */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

}  // namespace rt