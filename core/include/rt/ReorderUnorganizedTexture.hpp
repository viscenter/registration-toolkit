#pragma once

/** @file */

#include <opencv2/core.hpp>

#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace rt
{

/**
 * @class ReorderUnorganizedTexture
 * @brief Reorder an unorganized texture image into an organized texture using
 * 3D mapping information
 *
 * This algorithm reorganizes embedded color information from an input mesh
 * into a new, organized texture image. The effect is similar to taking a
 * snapshot from "above" the input mesh: faces that are adjacent in 3D are also
 * adjacent in the new 2D texture image.
 *
 * This algorithm assumes the input mesh (and therefore the resulting
 * reorganized texture) is roughly planar. The mesh is realigned such that its
 * shortest dimension is perpendicular to the XY plane. The algorithm samples
 * the XY plane into an image, creating one pixel every user-specified interval.
 * A Z-axis aligned ray through the pixel is used to calculate an intersection
 * and correspondence between the mesh and the new image. The color of the
 * intersected point is bilinearly interpolated from the original texture image
 * and placed into the pixel. A UV map is also generated that maps the original
 * input mesh to the new texture image.
 *
 */
class ReorderUnorganizedTexture
{
public:
    /**
     * Default distance (in mesh units) at which to sample the XY plane into
     * image
     */
    constexpr static double DEFAULT_SAMPLE_RATE{0.1};

    /** @brief Set the input mesh */
    void setMesh(const ITKMesh::Pointer& mesh);
    /** @brief Set the input UV map for the mesh */
    void setUVMap(const UVMap& uv);
    /** @brief Set the input, unorganized texture image */
    void setTextureMat(const cv::Mat& img);
    /**
     * @brief Set the rate (in mesh units) at which to sample XY plane into an
     * image
     */
    void setSampleRate(double s);
    /** @brief Set whether to use the first mesh intersection point */
    void setUseFirstIntersection(bool b);

    /** @brief Generate the new texture image and UV map */
    cv::Mat compute();

    /** @brief Get the output UV map */
    UVMap getUVMap();

    /** @brief Get the output texture image */
    cv::Mat getTextureMat();

private:
    /** Resample the input image into the organized texture */
    void create_texture_();
    /**
     * Generate a new UV map relating the input mesh to the organized texture
     */
    void create_uv_();

    /** Input mesh */
    ITKMesh::Pointer inputMesh_;
    /** Input UV map */
    UVMap inputUV_;
    /** Input texture image */
    cv::Mat inputTexture_;

    /** XY plane sample rate (in mesh units) */
    double sampleRate_{DEFAULT_SAMPLE_RATE};

    /** Origin position */
    cv::Vec3d origin_;
    /** First basis */
    cv::Vec3d xAxis_;
    /** Second basis */
    cv::Vec3d yAxis_;
    /** Third basis */
    cv::Vec3d zAxis_;

    /** Whether we want the first or last mesh interesection point */
    bool useFirstIntersection_{false};

    /** Output UV map */
    UVMap outputUV_;
    /** Output texture image */
    cv::Mat outputTexture_;
};
}  // namespace rt
