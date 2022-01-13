#pragma once

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

#include "rt/ReorderUnorganizedTexture.hpp"
#include "rt/filesystem.hpp"
#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace rt
{
namespace graph
{
/**
 * @brief Reorder unorganized texture
 * @see ReorderUnorganizedTexture
 */
class ReorderTextureNode : public smgl::Node
{
public:
    /** @see ReorderUnorganizedTexture::SamplingOrigin */
    using SamplingOrigin = ReorderUnorganizedTexture::SamplingOrigin;
    /** @see ReorderUnorganizedTexture::SamplingMode */
    using SamplingMode = ReorderUnorganizedTexture::SamplingMode;

    /** Default constructor */
    ReorderTextureNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Mesh port */
    smgl::InputPort<ITKMesh::Pointer> meshIn;
    /** @brief Input texture image port */
    smgl::InputPort<cv::Mat> imageIn;
    /** @brief Input UV Map port */
    smgl::InputPort<UVMap> uvMapIn;
    /** @copydoc ReorderUnorganizedTexture::samplingOrigin() */
    smgl::InputPort<SamplingOrigin> samplingOrigin;
    /** @copydoc ReorderUnorganizedTexture::samplingMode() */
    smgl::InputPort<SamplingMode> samplingMode;
    /** @copydoc ReorderUnorganizedTexture::setSampleRate() */
    smgl::InputPort<double> sampleRate;
    /** @copydoc ReorderUnorganizedTexture::setSampleDim() */
    smgl::InputPort<std::size_t> sampleDim;
    /** @copydoc ReorderUnorganizedTexture::setUseFirstIntersection() */
    smgl::InputPort<bool> useFirstIntersection;
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Output texture image port */
    smgl::OutputPort<cv::Mat> imageOut;
    /** @brief Output UV Map port */
    smgl::OutputPort<UVMap> uvMapOut;
    /**@}*/

private:
    /** Computation class */
    ReorderUnorganizedTexture reorder_;
    /** Output texture image */
    cv::Mat outImg_;
    /** Output UV map */
    UVMap outUV_;

    /** Graph serialize */
    auto serialize_(bool useCache, const filesystem::path& cacheDir)
        -> smgl::Metadata override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};
}  // namespace graph
}  // namespace rt
