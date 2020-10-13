#pragma once

#include <boost/filesystem.hpp>
#include <opencv2/core.hpp>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

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
 * @ingroup rtlib
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
    void setMesh(const vtkSmartPointer<vtkPolyData>& mesh);
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

    /**
     * Generate the barycentric coordinate of cartesian coordinate XYZ in
     * triangle ABC
     *
     * Note: Barycentric coordinates are relative to the first vertex. The
     * barycentric coordinate of XYZ in ABC is different from the barycentric
     * coordinate of XYZ in BAC.
     *
     */
    static cv::Vec3d XYZToBarycentric(
        cv::Vec3d XYZ, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C);

    /**
     * Generate the cartesian coordinate of barycentric coordinate UVW in
     * triangle ABC
     */
    static cv::Vec3d BarycentricToXYZ(
        cv::Vec3d UVW, cv::Vec3d A, cv::Vec3d B, cv::Vec3d C);

    /** Input mesh */
    vtkSmartPointer<vtkPolyData> inputMesh_;
    /** Input UV map */
    UVMap inputUV_;
    /** Input texture image */
    cv::Mat inputTexture_;

    /** XY plane sample rate (in mesh units) */
    double sampleRate_{DEFAULT_SAMPLE_RATE};

    cv::Vec3d origin_;
    cv::Vec3d xAxis_;
    cv::Vec3d yAxis_;
    cv::Vec3d zAxis_;

    /** Whether we want the first or last mesh interesection point */
    bool useFirstInterection_{false};

    /** Output UV map */
    UVMap outputUV_;
    /** Output texture image */
    cv::Mat outputTexture_;
};
}
