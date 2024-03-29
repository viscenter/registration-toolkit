#pragma once

/** @file */

#include <opencv2/core.hpp>

#include "rt/types/ITKMesh.hpp"
#include "rt/types/UVMap.hpp"

namespace rt
{

/**
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
     * @brief Bounding box corner to use for the origin of the sampling plane
     */
    enum class SamplingOrigin {
        TopLeft,    /** Set the sampling origin to the top-left corner of the
                       largest face of the bounding box */
        TopRight,   /** Set the sampling origin to the top-right corner of the
                       largest face of the bounding box */
        BottomLeft, /** Set the sampling origin to the bottom-left corner of the
                       largest face of the bounding box */
        BottomRight /** Set the sampling origin to the bottom-right corner of
                       the largest face of the bounding box */
    };

    /** @brief Sampling rate mode */
    enum class SamplingMode {
        Rate,         /** Use the sample rate provided by setSampleRate() */
        OutputWidth,  /**
                       * Calculate the sample rate needed to produce an output
                       * image with width defined by setSampleDim().
                       */
        OutputHeight, /**
                       * Calculate the sample rate needed to produce an output
                       * image with height defined by setSampleDim().
                       */
        AutoUV        /**
                       * Calculate a sample rate from the average pixel density in the
                       * UV mapped image.
                       */
    };

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

    /** @copydoc samplingOrigin() */
    void setSamplingOrigin(SamplingOrigin o);

    /** @brief Sampling bounding box origin */
    [[nodiscard]] auto samplingOrigin() const -> SamplingOrigin;

    /** @copydoc samplingMode() */
    void setSamplingMode(SamplingMode m);

    /** @brief Sampling rate mode */
    [[nodiscard]] auto samplingMode() const -> SamplingMode;

    /**
     * @brief The rate (in mesh units) at which to sample the image plane
     *
     * @see samplingMode()
     */
    void setSampleRate(double s);

    /** @copydoc setSampleRate() */
    [[nodiscard]] auto sampleRate() const -> double;

    /**
     * @brief The size of the output image in pixels
     *
     * Only used if setSamplingMode() is set to SampleMode::OutputWidth or
     * SampleMode::OutputHeight
     *
     * @see samplingMode()
     */
    void setSampleDim(std::size_t d);

    /** @copydoc setSampleDim() */
    [[nodiscard]] auto sampleDim() const -> std::size_t;

    /** @brief Whether to use the first mesh intersection point */
    void setUseFirstIntersection(bool b);

    /** @copydoc setUseFirstIntersection() */
    [[nodiscard]] auto useFirstIntersection() const -> bool;

    /** @brief Generate the new texture image and UV map */
    auto compute() -> cv::Mat;

    /** @brief Get the output UV map */
    auto getUVMap() -> UVMap;

    /** @brief Get the output texture image */
    auto getTextureMat() -> cv::Mat;

    /**
     * @brief Get depth map relative to the bounding box plane
     *
     * Depth is a floating point image in mesh units.
     */
    auto getDepthMap() -> cv::Mat;

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

    /** Sample origin */
    SamplingOrigin sampleOrigin_{SamplingOrigin::TopLeft};
    /** Sample mode */
    SamplingMode sampleMode_{SamplingMode::Rate};
    /** XY plane sample rate (in mesh units) */
    double sampleRate_{DEFAULT_SAMPLE_RATE};
    /** Length of the predefined sampling dimension */
    std::size_t sampleDim_{800};

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
    /** Output depth map */
    cv::Mat outputDepthMap_;
};
}  // namespace rt
