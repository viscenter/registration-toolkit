#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Graph.hpp>
#include <smgl/Node.hpp>

#include "rt/types/CompositeTransform.hpp"

namespace rt
{

/**
 * @brief Resample a moving image using a pre-generated transform. Output image
 * is of size s.
 *
 */
cv::Mat ImageTransformResampler(
    const cv::Mat& m,
    const cv::Size& s,
    const CompositeTransform::Pointer& transform);

namespace graph
{

class ImageResampleNode : public smgl::Node
{
public:
    ImageResampleNode();

    smgl::InputPort<cv::Mat> fixedImage{&fixed_};
    smgl::InputPort<cv::Mat> movingImage{&moving_};
    smgl::InputPort<CompositeTransform::Pointer> transform{&tfm_};
    smgl::InputPort<bool> forceAlpha{&forceAlpha_};
    smgl::OutputPort<cv::Mat> resampledImage{&resampled_};

private:
    bool forceAlpha_{false};
    cv::Mat fixed_;
    cv::Mat moving_;
    CompositeTransform::Pointer tfm_;
    cv::Mat resampled_;

    // TODO: Serialize
};

}  // namespace graph
}