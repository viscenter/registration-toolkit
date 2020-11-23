#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/DeformableRegistration.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/Transforms.hpp"

namespace rt
{
namespace graph
{

/** @brief DeformableRegistration Graph Node */
class DeformableRegistrationNode : public smgl::Node
{
public:
    /** Default constructor */
    DeformableRegistrationNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Fixed image port */
    smgl::InputPort<cv::Mat> fixedImage{
        &reg_, &DeformableRegistration::setFixedImage};
    /** @brief Moving image port */
    smgl::InputPort<cv::Mat> movingImage{
        &reg_, &DeformableRegistration::setMovingImage};
    /** @brief Deformable iterations port */
    smgl::InputPort<int> iterations{&iters_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Final transform port */
    smgl::OutputPort<Transform::Pointer> transform{&tfm_};
    /**@}*/

private:
    /** Registration method */
    DeformableRegistration reg_;
    /** Iterations */
    int iters_{DeformableRegistration::DEFAULT_ITERATIONS};
    /** Final transform */
    Transform::Pointer tfm_;
    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

}  // namespace graph
}  // namespace rt