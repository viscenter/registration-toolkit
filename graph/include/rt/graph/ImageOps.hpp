#pragma once

/** @file */

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/filesystem.hpp"

namespace rt::graph
{

/** @copybrief rt::ColorConvertImage */
class ColorConvertNode : public smgl::Node
{
public:
    /** Default constructor */
    ColorConvertNode();

    /** @brief Input image */
    smgl::InputPort<cv::Mat> imageIn;
    /** @brief Number of channels in the output image */
    smgl::InputPort<std::size_t> channels;
    /** @brief Output image */
    smgl::OutputPort<cv::Mat> imageOut;

private:
    /** Input image */
    cv::Mat input_;
    /** Channels */
    std::size_t cns_{3};
    /** Output image */
    cv::Mat output_;

    /** Graph serialize */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};

}  // namespace rt::graph