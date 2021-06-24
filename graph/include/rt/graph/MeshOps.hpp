#pragma once

#include <opencv2/core.hpp>
#include <smgl/Node.hpp>
#include <smgl/Ports.hpp>

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
    /** Default constructor */
    ReorderTextureNode();

    /** @name Input Ports */
    /**@{*/
    /** @brief Mesh port */
    smgl::InputPort<ITKMesh::Pointer> meshIn{&mesh_};
    /** @brief Input texture image port */
    smgl::InputPort<cv::Mat> imageIn{&inImg_};
    /** @brief Input UV Map port */
    smgl::InputPort<UVMap> uvMapIn{&inUV_};
    /** @brief Mesh domain sample rate */
    smgl::InputPort<double> sampleRate{&sample_rate_};
    /**@}*/

    /** @name Output Ports */
    /**@{*/
    /** @brief Output texture image port */
    smgl::OutputPort<cv::Mat> imageOut{&outImg_};
    /** @brief Output UV Map port */
    smgl::OutputPort<UVMap> uvMapOut{&outUV_};
    /**@}*/

private:
    /** Mesh */
    ITKMesh::Pointer mesh_;
    /** Input texture image */
    cv::Mat inImg_;
    /** Input UV map */
    UVMap inUV_;
    /** Sample rate */
    double sample_rate_;
    /** Output texture image */
    cv::Mat outImg_;
    /** Output UV map */
    UVMap outUV_;

    /** Graph serialize */
    smgl::Metadata serialize_(
        bool useCache, const filesystem::path& cacheDir) override;
    /** Graph deserialize */
    void deserialize_(
        const smgl::Metadata& meta, const filesystem::path& cacheDir) override;
};
}  // namespace graph
}  // namespace rt
